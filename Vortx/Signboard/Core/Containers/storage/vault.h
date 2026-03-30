#pragma once

#include "Signboard/Core/Math/bitops.h"

#include <vector>
#include <cstdint>
#include <type_traits>
#include <new>

namespace storage {

	namespace detail {
		template <typename Storage, typename T>
		class storage_readIterator;
	}

	template<typename T> class vault_readAccessor;
	template<typename T> class vault_writeAccessor;

	template <typename T>
	class vault {
	public:
		vault(uint32_t capacity = 0) {
			slots.reserve(capacity);
			aliveBits.reserve((capacity + 63) / 64);
		}

		~vault() {
			uint32_t size = static_cast<uint32_t>(aliveBits.size());
			uint32_t _slotC = static_cast<uint32_t>(slots.size());
			for (uint32_t w = 0; w < size; ++w) {
				uint64_t _wd = aliveBits[w];

				while (_wd) {
					uint32_t bit = bitops::ctz(_wd);
					uint32_t idx = w * 64 + bit;

					if (idx < _slotC) {
						slots[idx].object_ptr()->~T();
					}

					_wd &= _wd - 1;
				}
			}
			slots.clear();
			aliveBits.clear();
		}

	private:
		struct Storage {
			std::aligned_storage_t<sizeof(T), alignof(T)> storage;

			T* object_ptr() {
				return std::launder(reinterpret_cast<T*>(&storage));
			}

			const T* object_ptr() const {
				return std::launder(reinterpret_cast<const T*>(&storage));
			}
		};

		std::vector<Storage> slots;
		std::vector<uint64_t> aliveBits;
		uint32_t nextFreeHint = 0;

		uint32_t _alloc() {
			uint32_t _wdC = static_cast<uint32_t>(aliveBits.size());

			for (uint32_t w = nextFreeHint; w < _wdC; ++w) {
				uint64_t _wd = aliveBits[w];

				if (~_wd) {
					uint64_t _freeMk = ~_wd;
					uint32_t bit = bitops::ctz(_freeMk);

					uint32_t idx = w * 64 + bit;

					if (idx >= slots.size())
						slots.resize(idx + 1);

					nextFreeHint = w;
					return idx;
				}
			}

			for (uint32_t w = 0; w < nextFreeHint; ++w) {
				uint64_t _wd = aliveBits[w];

				if (~_wd) {
					uint64_t _freeMk = ~_wd;
					uint32_t bit = bitops::ctz(_freeMk);

					uint32_t idx = w * 64 + bit;
					
					if (idx >= slots.size())
						slots.resize(idx + 1);

					nextFreeHint = w;
					return idx;
				}
			}

			uint32_t idx = static_cast<uint32_t>(slots.size());
			slots.emplace_back();
			_ensure_cap(idx);
			nextFreeHint = idx / 64;

			return idx;
		}

		bool _is_alive(uint32_t index) const {
			uint32_t _wdi = index / 64;
			if (_wdi >= aliveBits.size()) return false;
			return (aliveBits[_wdi] >> (index % 64)) & 1ull;
		}

		void _ensure_cap(uint32_t index) {
			uint32_t _req = index / 64 + 1;
			if (aliveBits.size() < _req)
				aliveBits.resize(_req, 0);
		}

		void _set_alive(uint32_t index) {
			_ensure_cap(index);
			aliveBits[index / 64] |= (1ull << (index % 64));
		}

		void _set_dead(uint32_t index) {
			uint32_t _wdi = index / 64;
			if (_wdi < aliveBits.size())
				aliveBits[_wdi] &= ~(1ull << (index % 64));
		}

		friend class vault_writeAccessor<T>;
		friend class vault_readAccessor<T>;
		friend class detail::storage_readIterator<vault<T>, T>;

	};

}