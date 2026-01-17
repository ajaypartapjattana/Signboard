#pragma once

namespace storage {

	namespace detail {

		template <typename Storage, typename T>
		class StorageReadIterator {
		public:
			StorageReadIterator(const Storage& storage, size_t index)
				: m_storage(storage), m_index(index)
			{
				skipDead();
			}

			const T& operator*() const {
				return m_storage.slots[m_index].object;
			}

			StorageReadIterator& operator++() {
				++m_index;
				skipDead();
				return *this;
			}

			bool operator!=(const StorageReadIterator& other) const {
				return m_index != other.m_index;
			}

		private:
			void skipDead() {
				while (m_index < m_storage.slots.size() && !m_storage.slots[m_index].alive)
					++m_index;
			}

		private:
			const Storage& m_storage;
			size_t m_index;

		};

	}

}