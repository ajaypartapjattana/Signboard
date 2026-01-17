#pragma once

#include "storage_vault.h"
#include "storage_handle.h"

namespace storage {

	template <typename T>
	class VaultWriteAccessor {
	public:
		VaultWriteAccessor(Vault<T>& vault)
			: m_vault(vault) {
		}

		VaultWriteAccessor(const VaultWriteAccessor&) = delete;
		VaultWriteAccessor& operator=(const VaultWriteAccessor&) = delete;

		template <typename... Args>
		Handle create(Args&&... args) {
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

		void destroy(Handle h) {
			auto& slot = m_vault.slots[h.index];

			if (!slot.alive || slot.generation != h.generation)
				return;

			slot.alive = false;
			slot.generation++;
			m_vault.freeList.push_back(h.index);
		}

	private:
		Vault<T>& m_vault;
	};

	template <typename T>
	class VaultReadAccessor {
	public:
		explicit VaultReadAccessor(const Vault<T>& vault)
			: m_vault(vault) {
		}

		VaultReadAccessor(const VaultReadAccessor&) = delete;
		VaultReadAccessor& operator=(const VaultReadAccessor&) = delete;

		const T* get(Handle h) const {
			const auto& slot = m_vault.slots[h.index];

			if (!slot.alive || slot.generation != h.generation)
				return nullptr;

			return &slot.object;
		}

		auto begin() const { return detail::StorageReadIterator<Vault<T>, T>(m_vault, 0); }
		auto end() const { return detail::StorageReadIterator<Vault<T>, T>(m_vault, m_vault.slots.size()); }

	private:
		const Vault<T>& m_vault;

	};

}