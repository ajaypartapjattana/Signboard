#pragma once

#include <iterator>
#include <cstddef>

namespace storage::detail {

	template <typename Storage, typename T>
	class storage_readIterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = const T*;
		using reference = const T&;

		storage_readIterator(const Storage& storage, std::size_t index)
			: m_storage(&storage), m_index(index)
		{
			skipDead();
		}

		reference operator*() const {
			return *m_storage->slots[m_index].object_ptr();
		}

		pointer operator->() const {
			return m_storage->slots[m_index].object_ptr();
		}

		storage_readIterator& operator++() {
			++m_index;
			skipDead();
			return *this;
		}

		storage_readIterator operator++(int) {
			auto tmp = *this;
			++(*this);
			return tmp;
		}

		bool operator==(const storage_readIterator& other) const {
			return m_index == other.m_index &&
				m_storage == other.m_storage;
		}

		bool operator!=(const storage_readIterator& other) const {
			return !(*this == other);
		}

	private:
		void skipDead() {
			while (m_index < m_storage->slots.size() &&
				!m_storage->slots[m_index].alive)
				++m_index;
		}

	private:
		const Storage* m_storage;
		std::size_t m_index;
	};

}