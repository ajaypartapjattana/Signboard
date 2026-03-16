#pragma once

#include "detail/storage_detail.h"

#include <vector>
#include <cstdint>
#include <type_traits>

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

		~vault() {
			for (auto& slot : slots) {
				if (slot.alive) {
					slot.object_ptr()->~T();
					slot.alive = false;
				}
			}

			slots.clear();
		}

	private:
		struct Slot {
			std::aligned_storage_t<sizeof(T), alignof(T)> storage;
			uint32_t generation = 0;
			bool alive = false;

			T* object_ptr() {
				return std::launder(reinterpret_cast<T*>(&storage));
			}

			const T* object_ptr() const {
				return std::launder(reinterpret_cast<const T*>(&storage));
			}
		};

		std::vector<Slot> slots;
		std::vector<uint32_t> freeList;

		friend class vault_writeAccessor<T>;
		friend class vault_readAccessor<T>;

		template <typename Storage, typename U> 
		friend class detail::storage_readIterator;

	};

}