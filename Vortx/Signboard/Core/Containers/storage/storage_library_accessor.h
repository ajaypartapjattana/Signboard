#pragma once

#include "storage_library.h"
#include "storage_handle.h"

namespace storage {

	template <typename T>
	class LibraryWriteAccessor {
	public:
		explicit LibraryWriteAccessor(Library<T>& library)
			: m_library(library) {}

		LibraryWriteAccessor(const LibraryWriteAccessor&) = delete;
		LibraryWriteAccessor& operator=(const LibraryWriteAccessor&) = delete;

		template <typename... Args>
		Handle findOrCreate(uint64_t hash, Args&&... args) {
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

			Handle h{ index, slot.generation };
			m_library.hashMap[hash] = h;

			return h;
		}

		void destroy(Handle h) {
			auto& slot = m_library.slots[h.index];

			if (!slot.alive || slot.generation != h.generation)
				return;

			m_library.hashMap.erase(slot.hash);

			slot.alive = false;
			slot.generation++;
			m_library.freeList.push_back(h.index);
		}

	private:
		Library<T>& m_library;
	};

	template <typename T>
	class LibraryReadAccessor {
	public:
		LibraryReadAccessor(const Library<T>& library)
			: m_library(library) {}

		LibraryReadAccessor(const LibraryReadAccessor&) = delete;
		LibraryReadAccessor& operator=(const LibraryReadAccessor&) = delete;

		const T* get(Handle h) const {
			auto& slot = m_library.slots[h.index];

			if (!slot.alive || slot.generation != h.generation)
				return nullptr;

			return &slot.object;
		}

		auto begin() const { return detail::StorageReadIterator<Library<T>, T>(m_library, 0); }
		auto end() const { return detail::StorageReadIterator<Library<T>, T>(m_library, m_library.slots.size()); }

	private:
		const Library<T>& m_library;

	};

}