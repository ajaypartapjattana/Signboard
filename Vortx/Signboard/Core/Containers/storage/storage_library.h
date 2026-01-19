#pragma once

#include "storage_handle.h"
#include "detail/storage_detail.h"

#include <vector>
#include <unordered_map>

namespace storage {

	template <typename T> class library_writeAccessor;
	template <typename T> class library_readAccessor;

	template <typename T>
	class library {
	public:
		library(uint32_t capacity = 0) {
			slots.reserve(capacity);
			freeList.reserve(capacity);
		}

	private:
		struct HashedSlot {
			T object;
			uint32_t hash;
			uint32_t generation = 0;
			bool alive = false;
		};

		std::vector<HashedSlot> slots;
		std::vector<uint32_t> freeList;
		std::unordered_map<uint64_t, storage::storage_handle> hashMap;

		friend class library_writeAccessor<T>;
		friend class library_readAccessor<T>;

		template <typename Storage, typename U>
		friend class detail::storage_readIterator;

	};

}