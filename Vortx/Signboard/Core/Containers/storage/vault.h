#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <new>
#include <type_traits>
#include <vector>

#include "Signboard/Core/Math/bitops.h"

namespace ctnr {

	template <typename Storage, typename T> class storage_readIterator;

	template<typename T> class vault_readAccessor;
	template<typename T> class vault_writeAccessor;

	template <typename T>
	class vault {
	public:
		vault(uint32_t capacity = 0) noexcept {
			slots.reserve(capacity);
			aliveBits.reserve((capacity + 63) / 64);
		}

		vault(const vault&) = delete;
		vault& operator=(const vault&) = delete;
		
		vault(vault&&) noexcept = default;
		vault& operator=(vault&&) noexcept = default;

		~vault() noexcept(std::is_nothrow_destructible_v<T>) {
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

		_NODISCARD uint32_t _alloc() {
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

		bool _is_alive(uint32_t index) const noexcept {
			uint32_t _wdi = index / 64;
			if (_wdi >= aliveBits.size()) return false;
			return (aliveBits[_wdi] >> (index % 64)) & 1ull;
		}

		void _ensure_cap(uint32_t index) noexcept {
			uint32_t _req = index / 64 + 1;
			if (aliveBits.size() < _req)
				aliveBits.resize(_req, 0);
		}

		void _set_alive(uint32_t index) noexcept {
			_ensure_cap(index);
			aliveBits[index / 64] |= (1ull << (index % 64));
		}

		void _set_dead(uint32_t index) {
			uint32_t _wdi = index / 64;
			if (_wdi < aliveBits.size())
				aliveBits[_wdi] &= ~(1ull << (index % 64));
		}

		friend class storage_readIterator<vault<T>, T>;

		friend class vault_writeAccessor<T>;
		friend class vault_readAccessor<T>;

	};

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
				m_currentIndex == other.m_currentIndex &&
				m_wordIndex == other.m_wordIndex;
		}

		bool operator!=(const storage_readIterator& other) const {
			return !(*this == other);
		}

	private:
		__forceinline bool _next_non_empty_word(const Storage* storage, uint32_t wordCount) {
			while (m_currentWord == 0) {
				++m_wordIndex;

				if (m_wordIndex >= wordCount) {
					m_currentIndex = static_cast<uint32_t>(-1);
					return false;
				}

				m_currentWord = storage->aliveBits[m_wordIndex];
				m_baseIndex = m_wordIndex * 64;
			}
			return true;
		}

		__forceinline void _advance_to_next() {
			const auto* storage = m_storage;
			const uint32_t _slC = static_cast<uint32_t>(storage->slots.size());
			const uint32_t _wdC = static_cast<uint32_t>(storage->aliveBits.size());

			if (m_currentWord)
				m_currentWord &= m_currentWord - 1;

			if (!_next_non_empty_word(storage, _wdC))
				return;

			while (true) {
				uint64_t word = m_currentWord;
				uint32_t bit = bitops::ctz(word);
				m_currentIndex = m_baseIndex + bit;

				if (m_currentIndex + 1 < _slC) {
					_mm_prefetch(
						reinterpret_cast<const char*>(&storage->slots[m_currentIndex + 1]),
						_MM_HINT_T0
					);
				}

				if (m_currentIndex < _slC)
					return;

				m_currentWord &= m_currentWord - 1;

				if (!_next_non_empty_word(storage, _wdC))
					return;
			}
		}

	private:
		const Storage* m_storage = nullptr;

		uint32_t m_wordIndex = 0;
		uint64_t m_currentWord = 0;
		uint32_t m_baseIndex = 0;

		uint32_t m_currentIndex = static_cast<uint32_t>(-1);

	};

	template <typename T>
	class vault_readAccessor {
	public:
		explicit vault_readAccessor(const vault<T>& vault)
			:
			m_vault(vault)
		{

		}

		vault_readAccessor(const vault_readAccessor&) = delete;
		vault_readAccessor& operator=(const vault_readAccessor&) = delete;

		vault_readAccessor(vault_readAccessor&& other) noexcept
			:
			m_vault(other.m_vault)
		{

		}

		vault_readAccessor& operator=(vault_readAccessor&& other) = delete;

		_NODISCARD const T* get(uint32_t index) const noexcept {
			if (!m_vault._is_alive(index)) return nullptr;
			return m_vault.slots[index].object_ptr();
		}

		inline auto begin() const {
			return detail::storage_readIterator<vault<T>, T>(m_vault, 0);
		}

		inline auto end() const {
			return detail::storage_readIterator<vault<T>, T>(m_vault, static_cast<uint32_t>(m_vault.aliveBits.size()));
		}

	private:
		const vault<T>& m_vault;

	};

	template <typename T>
	class vault_writeAccessor {
	public:
		vault_writeAccessor(vault<T>& vault)
			:
			m_vault(vault)
		{

		}

		vault_writeAccessor(const vault_writeAccessor&) = delete;
		vault_writeAccessor& operator=(const vault_writeAccessor&) = delete;

		vault_writeAccessor(vault_writeAccessor&& other) noexcept
			:
			m_vault(other.m_vault)
		{

		}

		vault_writeAccessor& operator=(vault_writeAccessor&& other) = delete;

		void reserve_slots(uint32_t count) {
			if (count == 0) return;

			uint32_t _start = static_cast<uint32_t>(m_vault.slots.size());

			m_vault.slots.resize(_start + count);
			m_vault._ensure_cap(_start + count - 1);
		}

		template<typename F>
		_NODISCARD uint32_t construct(F&& builder) noexcept(std::is_nothrow_constructible_v<T> && noexcept(builder(std::declval<T*>()))) {
			uint32_t idx = m_vault._alloc();

			T* obj = m_vault.slots[idx].object_ptr();

			new (obj) T();

			try {
				builder(obj);
				m_vault._set_alive(idx);
			}
			catch (...) {
				obj->~T();
				throw;
			}

			return idx;
		}

		template <typename... Args>
		_NODISCARD uint32_t create(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>) {
			uint32_t index = m_vault._alloc();

			T* obj = m_vault.slots[index].object_ptr();

			new (obj) T(std::forward<Args>(args)...);
			m_vault._set_alive(index);

			return index;
		}

		void destroy(uint32_t index) noexcept(std::is_nothrow_destructible_v<T>) {
			if (!m_vault._is_alive(index))
				return;

			m_vault.slots[index].object_ptr()->~T();
			m_vault._set_dead(index);
		}

		void clear() {
			uint32_t _sz = static_cast<uint32_t>(m_vault.aliveBits.size());
			uint32_t _slC = static_cast<uint32_t>(m_vault.slots.size());

			for (uint32_t w = 0; w < _sz; ++w) {
				uint64_t _wd = m_vault.aliveBits[w];

				while (_wd) {
					uint32_t bit = bitops::ctz(_wd);
					uint32_t idx = w * 64 + bit;

					if (idx < _slC) {
						m_vault.slots[idx].object_ptr()->~T();
					}

					_wd &= _wd - 1;
				}

			}

			m_vault.slots.clear();
			m_vault.aliveBits.clear();
			m_vault.nextFreeHint = 0;
		}

		_NODISCARD T* get(uint32_t index) const noexcept {
			if (!m_vault._is_alive(index)) return nullptr;
			return m_vault.slots[index].object_ptr();
		}

	private:
		vault<T>& m_vault;

	};

}
