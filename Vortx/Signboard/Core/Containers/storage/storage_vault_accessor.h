#pragma once

#include "storage_vault.h"
#include "storage_handle.h"

namespace storage {

	template <typename T>
	class vault_writeAccessor {
	public:
		vault_writeAccessor(vault<T>& vault)
			: m_vault(vault) {
		}

		vault_writeAccessor(const vault_writeAccessor&) = delete;
		vault_writeAccessor& operator=(const vault_writeAccessor&) = delete;

		template <typename... Args>
		storage_handle create(Args&&... args) {
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
			slot.object = T(std::forward<Args>(args)...);
			slot.alive = true;

			return { index, slot.generation };
		}

		void destroy(storage_handle h) {
			auto& slot = m_vault.slots[h.index];

			if (!slot.alive || slot.generation != h.generation)
				return;

			slot.alive = false;
			slot.generation++;
			m_vault.freeList.push_back(h.index);
		}

	private:
		vault<T>& m_vault;
	};

	template <typename T>
	class vault_readAccessor {
	public:
		explicit vault_readAccessor(const vault<T>& vault)
			: m_vault(vault) {
		}

		vault_readAccessor(const vault_readAccessor&) = delete;
		vault_readAccessor& operator=(const vault_readAccessor&) = delete;

		const T* get(storage_handle h) const {
			const auto& slot = m_vault.slots[h.index];

			if (!slot.alive || slot.generation != h.generation)
				return nullptr;

			return &slot.object;
		}

		auto begin() const { return detail::storage_readIterator<vault<T>, T>(m_vault, 0); }
		auto end() const { return detail::storage_readIterator<vault<T>, T>(m_vault, m_vault.slots.size()); }

	private:
		const vault<T>& m_vault;

	};

}