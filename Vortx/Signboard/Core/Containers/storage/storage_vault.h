#pragma once

#include "detail/storage_detail.h"

#include <vector>
#include <cstdint>

namespace storage {

	template<typename T> class vault_readAccessor;
	template<typename T> class vault_writeAccessor;

	template <typename T>
	class vault {
	public:
		vault(uint32_t capacity = 0) {
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

		friend class vault_writeAccessor<T>;
		friend class vault_readAccessor<T>;

		template <typename Storage, typename U> 
		friend class detail::storage_readIterator;

	};

}