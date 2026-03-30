#pragma once

#include "vault.h"
#include "vault_iterator.h"

#include "Signboard/Core/Math/bitops.h"

using handle = uint32_t;

namespace storage {

	template <typename T>
	class vault_readAccessor {
	public:
		explicit vault_readAccessor(const vault<T>& vault)
			: 
			m_vault(vault) 
		{

		}

		vault_readAccessor(const vault_readAccessor&) = delete;
		vault_readAccessor& operator=(const vault_readAccessor&) = delete;

		vault_readAccessor(vault_readAccessor&& other) noexcept 
			: 
			m_vault(other.m_vault)
		{

		}

		vault_readAccessor& operator=(vault_readAccessor&& other) = delete;

		_NODISCARD const T* get(uint32_t index) const noexcept {
			if (!m_vault._is_alive(index)) return nullptr;
			return m_vault.slots[index].object_ptr();
		}

		inline auto begin() const { 
			return detail::storage_readIterator<vault<T>, T>(m_vault, 0);
		}

		inline auto end() const { 
			return detail::storage_readIterator<vault<T>, T>(m_vault, static_cast<uint32_t>(m_vault.aliveBits.size())); 
		}

	private:
		const vault<T>& m_vault;

	};

	template <typename T>
	class vault_writeAccessor {
	public:
		vault_writeAccessor(vault<T>& vault)
			: 
			m_vault(vault) 
		{

		}

		vault_writeAccessor(const vault_writeAccessor&) = delete;
		vault_writeAccessor& operator=(const vault_writeAccessor&) = delete;

		vault_writeAccessor(vault_writeAccessor&& other) noexcept 
			: 
			m_vault(other.m_vault)
		{

		}

		vault_writeAccessor& operator=(vault_writeAccessor&& other) = delete;

		void create_empty(uint32_t count) {
			if (count == 0) return;

			uint32_t _start = static_cast<uint32_t>(m_vault.slots.size());
			
			m_vault.slots.resize(_start + count);
			m_vault._ensure_cap(_start + count - 1);
		}

		template<typename F>
		_NODISCARD uint32_t construct(F&& builder) noexcept(noexcept(builder(std::declval<T*>()))) {
			uint32_t idx = m_vault._alloc();

			T* obj = m_vault.slots[idx].object_ptr();

			new (obj) T();

			try {
				builder(obj);
				m_vault._set_alive(idx);
			}
			catch (...) {
				obj->~T();
				throw;
			}

			return idx;
		}

		template <typename... Args>
		_NODISCARD uint32_t create(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>) {
			uint32_t index = m_vault._alloc();

			T* obj = m_vault.slots[index].object_ptr();

			new (obj) T(std::forward<Args>(args)...);
			m_vault._set_alive(index);

			return index;
		}

		void destroy(uint32_t index) noexcept(std::is_nothrow_destructible_v<T>) {
			if (!m_vault._is_alive(index))
				return;

			m_vault.slots[index].object_ptr()->~T();
			m_vault._set_dead(index);
		}

		void clear() {
			uint32_t _sz = static_cast<uint32_t>(m_vault.aliveBits.size());
			uint32_t _slC = static_cast<uint32_t>(m_vault.slots.size());

			for (uint32_t w = 0; w < _sz; ++w) {
				uint64_t _wd = m_vault.aliveBits[w];

				while (_wd) {
					uint32_t bit = bitops::ctz(_wd);
					uint32_t idx = w * 64 + bit;

					if (idx < _slC) {
						m_vault.slots[idx].object_ptr()->~T();
					}

					_wd &= _wd - 1;
				}

			}
			
			m_vault.slots.clear();
			m_vault.aliveBits.clear();
			m_vault.nextFreeHint = 0;
		}

		_NODISCARD T* get(uint32_t index) const noexcept {
			if (!m_vault._is_alive(index)) return nullptr;
			return m_vault.slots[index].object_ptr();
		}

	private:
		vault<T>& m_vault;

	};

}