#pragma once

#include "detail/storage_detail.h"

#include <vector>
#include <cstdint>

namespace storage {

	template<typename T> class VaultReadAccessor;
	template<typename T> class VaultWriteAccessor;

	template <typename T>
	class Vault {
	public:
		Vault(uint32_t capacity = 0) {
			slots.reserve(capacity);
			freeList.reserve(capacity);
		}

	private:
		struct Slot {
			T object;
			uint32_t generation = 0;
			bool alive = false;
		};

		std::vector<Slot> slots;
		std::vector<uint32_t> freeList;

		friend class VaultWriteAccessor<T>;
		friend class VaultReadAccessor<T>;

		template <typename Storage, typename U> 
		friend class detail::StorageReadIterator;

	};

}