#pragma once

#include "storage_vault.h"
#include "storage_handle.h"

namespace storage {

	template <typename T>
	class vault_readAccessor {
	public:
		explicit vault_readAccessor(const vault<T>& vault)
			: m_vault(vault) 
		{

		}

		vault_readAccessor(const vault_readAccessor&) = delete;
		vault_readAccessor& operator=(const vault_readAccessor&) = delete;

		vault_readAccessor(vault_readAccessor&& other) noexcept 
			: m_vault(other.m_vault)
		{

		}

		vault_readAccessor& operator=(vault_readAccessor&& other) noexcept {
			if (this == &other)
				return *this;

			m_vault = other.m_vault;

			return *this;
		}

		_NODISCARD const T* 
		get(storage_handle h) const noexcept 
		{
			const auto& slot = m_vault.slots[h.index];
			return (slot.alive && slot.generation == h.generation)
				? slot.object_ptr() 
				: nullptr;
		}

		inline auto begin() const { return detail::storage_readIterator<vault<T>, T>(m_vault, 0); }
		inline auto end() const { return detail::storage_readIterator<vault<T>, T>(m_vault, m_vault.slots.size()); }

	private:
		const vault<T>& m_vault;

	};

	template <typename T>
	class vault_writeAccessor {
	public:
		vault_writeAccessor(vault<T>& vault)
			: m_vault(vault) 
		{

		}

		vault_writeAccessor(const vault_writeAccessor&) = delete;
		vault_writeAccessor& operator=(const vault_writeAccessor&) = delete;

		vault_writeAccessor(vault_writeAccessor&& other) noexcept 
			: m_vault(other.m_vault)
		{

		}

		vault_writeAccessor& operator=(vault_writeAccessor&& other) = delete;

		void create_empty(uint32_t count) {
			uint32_t start = static_cast<uint32_t>(m_vault.slots.size());
			
			m_vault.slots.resize(start + count);

			m_vault.freeList.reserve(m_vault.freeList.size() + count);
			for (uint32_t i = 0; i < count; ++i) {
				uint32_t index = start + i;

				auto& slot = m_vault.slots[index];
				slot.alive = false;

				m_vault.freeList.push_back(index);
			}
		}

		void clear() {
			for (auto& slot : m_vault.slots) {
				if (slot.alive) {
					slot.object_ptr()->~T();
					slot.alive = false;
				}
			}
			m_vault.slots.clear();
		}

		template<typename F>
		_NODISCARD storage_handle construct(F&& builder) noexcept(noexcept(builder(std::declval<T*>()))) {
			uint32_t index;

			if (!m_vault.freeList.empty()) {
				index = m_vault.freeList.back();
				m_vault.freeList.pop_back();
			}
			else {
				index = static_cast<uint32_t>(m_vault.slots.size());
				m_vault.slots.emplace_back();
			}

			auto& slot = m_vault.slots[index];

			T* obj = slot.object_ptr();

			new (obj) T();

			try {
				builder(obj);
				slot.alive = true;
			} catch (...) {
				obj->~T();
				throw;
			}
			
			slot.alive = true;
			return{ index, slot.generation };
		}

		template <typename... Args>
		_NODISCARD storage_handle create(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>) {
			uint32_t index;

			if (!m_vault.freeList.empty()) {
				index = m_vault.freeList.back();
				m_vault.freeList.pop_back();
			}
			else {
				index = static_cast<uint32_t>(m_vault.slots.size());
				m_vault.slots.emplace_back();
			}

			auto& slot = m_vault.slots[index];
			new (slot.object_ptr()) T(std::forward<Args>(args)...);
			slot.alive = true;

			return { index, slot.generation };
		}

		void destroy(storage_handle h) noexcept(std::is_nothrow_destructible_v<T>) {
			auto& slot = m_vault.slots[h.index];

			if (!slot.alive || slot.generation != h.generation) 
				return;

			slot.object_ptr()->~T();
			slot.alive = false;
			slot.generation++;
			m_vault.freeList.push_back(h.index);
		}

		_NODISCARD T* get(storage_handle h) const noexcept {
			auto& slot = m_vault.slots[h.index];
			return slot.alive && slot.generation == h.generation ? slot.object_ptr() : nullptr;
		}

	private:
		vault<T>& m_vault;
	};

}