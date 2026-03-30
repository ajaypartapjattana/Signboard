#pragma once

#include "vault.h"
#include "Signboard/Core/Math/bitops.h"

#include <iterator>
#include <cstddef>
#include <cstdint>

namespace storage::detail {

	template <typename Storage, typename T>
	class storage_readIterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = const T*;
		using reference = const T&;

		storage_readIterator(const Storage& storage, uint32_t wordIndex)
			: 
			m_storage(&storage), 
			m_wordIndex(wordIndex)
		{
			if (m_wordIndex < m_storage->aliveBits.size()) {
				m_currentWord = m_storage->aliveBits[m_wordIndex];
				m_baseIndex = m_wordIndex * 64;
				_advance_to_next();
			}
		}

		reference operator*() const {
			return *m_storage->slots[m_currentIndex].object_ptr();
		}

		pointer operator->() const {
			return m_storage->slots[m_currentIndex].object_ptr();
		}

		storage_readIterator& operator++() {
			m_currentWord &= m_currentWord - 1;
			_advance_to_next();

			return *this;
		}

		storage_readIterator operator++(int) {
			auto tmp = *this;
			++(*this);
			return tmp;
		}

		bool operator==(const storage_readIterator& other) const {
			return m_storage == other.m_storage &&
				m_currentIndex == other.m_currentIndex;
		}

		bool operator!=(const storage_readIterator& other) const {
			return !(*this == other);
		}

	private:
		void _advance_to_next() {
			uint32_t _slC = static_cast<uint32_t>(m_storage->slots.size());
			while (true) {
				if (m_currentWord) {
					uint32_t bit = bitops::ctz(m_currentWord);
					m_currentIndex = m_baseIndex + bit;

					if (m_currentIndex < _slC)
						return;

					m_currentWord &= m_currentWord - 1;
				}
				else {
					++m_wordIndex;

					if (m_wordIndex >= m_storage->aliveBits.size()) {
						m_currentIndex = static_cast<uint32_t>(-1);
						return;
					}

					m_currentWord = m_storage->aliveBits[m_wordIndex];
					m_baseIndex = m_wordIndex * 64;
				}
			}
		}

	private:
		const Storage* m_storage = nullptr;
		
		uint32_t m_wordIndex = 0;
		uint64_t m_currentWord = 0;
		uint32_t m_baseIndex = 0;

		uint32_t m_currentIndex = static_cast<uint32_t>(-1);

	};

}