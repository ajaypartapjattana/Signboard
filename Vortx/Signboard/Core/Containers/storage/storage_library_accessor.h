#pragma once

#include "storage_library.h"
#include "storage_handle.h"

namespace storage {

	template <typename T>
	class library_readAccessor {
	public:
		library_readAccessor(const library<T>& library)
			: m_library(library) {
		}

		library_readAccessor(const library_readAccessor&) = delete;
		library_readAccessor& operator=(const library_readAccessor&) = delete;

		const T* get(storage_handle h) const {
			auto& slot = m_library.slots[h.index];

			if (!slot.alive || slot.generation != h.generation)
				return nullptr;

			return &slot.object;
		}

		auto begin() const { return detail::storage_readIterator<library<T>, T>(m_library, 0); }
		auto end() const { return detail::storage_readIterator<library<T>, T>(m_library, m_library.slots.size()); }

	private:
		const library<T>& m_library;

	};

	template <typename T>
	class library_writeAccessor {
	public:
		explicit library_writeAccessor(library<T>& library)
			: m_library(library) {
		}

		library_writeAccessor(const library_writeAccessor&) = delete;
		library_writeAccessor& operator=(const library_writeAccessor&) = delete;

		template <typename... Args>
		storage_handle findOrCreate(uint64_t hash, Args&&... args) {
			auto it = m_library.hashMap.find(hash);
			if (it != m_library.hashMap.end())
				return it->second;

			uint32_t index;

			if (!m_library.freeList.empty()) {
				index = m_library.freeList.back();
				m_library.freeList.pop_back();
			}
			else {
				index = static_cast<uint32_t>(m_library.slots.size());
				m_library.slots.emplace_back();
			}

			auto& slot = m_library.slots[index];
			slot.object = T(std::forward<Args>(args)...);
			slot.hash = hash;
			slot.alive = true;

			storage_handle h{ index, slot.generation };
			m_library.hashMap[hash] = h;

			return h;
		}

		void destroy(storage_handle h) {
			auto& slot = m_library.slots[h.index];

			if (!slot.alive || slot.generation != h.generation)
				return;

			m_library.hashMap.erase(slot.hash);

			slot.alive = false;
			slot.generation++;
			m_library.freeList.push_back(h.index);
		}

	private:
		library<T>& m_library;
	};

}