#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <cstring>
#include <cstdint>
#include <utility>
#include <stdexcept>
#include <cassert>
#include <type_traits>

namespace mem {

	constexpr size_t alignUp(size_t _Offset, size_t _Align) noexcept {
		return (_Offset + _Align - 1) & ~(_Align - 1);
	}

	constexpr size_t alignDown(size_t _Offset, size_t _Align) noexcept {
		return _Offset & ~(_Align - 1);
	}

	template <typename T>
	constexpr T* alignUp(T* _pAddr, size_t _Align) noexcept {
		return reinterpret_cast<T*>((reinterpret_cast<size_t>(_pAddr) + _Align - 1) & ~(_Align - 1));
	}

	template <typename T>
	constexpr T* alignDown(T* _pAddr, size_t _Align) noexcept {
		return reinterpret_cast<T*>(reinterpret_cast<size_t>(_pAddr) & ~(_Align - 1));
	}

	template <typename _Ty = uint8_t>
	class span {
	protected:
		_Ty* pBegin = nullptr;
		_Ty* pEnd = nullptr;

		bool adjacent(const span& _Other) const noexcept {
			return pEnd == _Other.pBegin || pBegin == _Other.pEnd;
		}

	public:
		span() noexcept = default;
		span(_Ty* _pBegin, _Ty* _pEnd) noexcept
			:
			pBegin(_pBegin),
			pEnd(_pEnd)
		{

		}
		span(_Ty* _pBegin, size_t _Count) noexcept
			:
			pBegin(_pBegin),
			pEnd(_pBegin + _Count)
		{

		}

		_Ty& operator[](size_t _Index) noexcept {
			return pBegin[_Index];
		}

		const _Ty& operator[](size_t _Index) const noexcept {
			return pBegin[_Index];
		}

		_Ty* begin() noexcept {
			return pBegin; 
		}

		const _Ty* begin() const noexcept { 
			return pBegin; 
		}

		_Ty* end() noexcept { 
			return pEnd; 
		}

		const _Ty* end() const noexcept { 
			return pEnd; 
		}

		_Ty* data() noexcept {
			return pBegin;
		}

		const _Ty* data() const noexcept {
			return pBegin;
		}

		size_t size() const noexcept {
			return static_cast<size_t>(pEnd - pBegin);
		}

		size_t index(_Ty* const _pAddr) const noexcept {
			return static_cast<size_t>(_pAddr - pBegin);
		}

		operator bool() const noexcept {
			return pBegin && pEnd;
		}

		bool empty() const noexcept {
			return pBegin == pEnd;
		}

		bool contains(_Ty* _Addr) const noexcept {
			return _Addr >= pBegin && _Addr < pEnd;
		}

		bool contains(const span& _Other) const noexcept {
			return _Other.pBegin >= pBegin && _Other.pEnd <= pEnd;
		}

		bool overlaps(const span& _Other) const noexcept {
			return pBegin < _Other.pEnd && _Other.pBegin < pEnd;
		}

		void assign(const _Ty& _Val) const noexcept(std::is_nothrow_copy_assignable_v<_Ty>) {
			std::fill(pBegin, pEnd, _Val);
		}

		void defaultConstruct() const noexcept {
			if constexpr (std::is_trivially_constructible_v<_Ty> && std::is_trivially_destructible_v<_Ty>) {
				memset(pBegin, 0, static_cast<size_t>(pEnd - pBegin));
			}
			else {
				for (_Ty* ptr{ pBegin }; ptr != pEnd; ++p)
					::new (ptr) _Ty{};
			}
		}

		_NODISCARD span slice(size_t _Size) noexcept {
			_Ty* _pSliceEnd = pBegin + _Size;
			_pSliceEnd = _pSliceEnd < pEnd ? _pSliceEnd : pEnd;

			span span{ pBegin, _pSliceEnd };
			pBegin = _pSliceEnd;

			return span;
		}

		void merge(const span& _Other) noexcept {
			if (!adjacent(_Other) && !overlaps(_Other))
				return;

			pBegin = pBegin < _Other.pBegin ? pBegin : _Other.pBegin;
			pEnd = pEnd > _Other.pEnd ? pEnd : _Other.pEnd;
		}

	};

	class memBase {
	protected:
		struct SystemMemoryInfo {
			size_t pageSize;
			size_t reservationGranularity;
		};

		inline static const SystemMemoryInfo memInfo = [] {
			SYSTEM_INFO info;
			GetSystemInfo(&info);

			return SystemMemoryInfo{
				(size_t)info.dwPageSize,
				(size_t)info.dwAllocationGranularity
			};
		}();

	};

	class stack : memBase {
	private:
		uint8_t* pBase = nullptr;
		uint8_t* pMark = nullptr;
		uint8_t* pCurrent = nullptr;
		uint8_t* pEnd = nullptr;
		uint8_t* pCap = nullptr;

		struct AllocHead {
			uint32_t prevHead;
			uint32_t extent;
		};

		constexpr static uint32_t headRatio = sizeof(AllocHead) / alignof(AllocHead);

		AllocHead* pOldHead = nullptr;

		template <typename _Ty>
		AllocHead* locateHead(_Ty* pAlloc) const noexcept {
			return reinterpret_cast<AllocHead*>(alignDown(reinterpret_cast<uint8_t>(pAlloc) - sizeof(AllocHead), sizeof(AllocHead)));
		}

		template <typename _Ty>
		span<_Ty> getAlloc(AllocHead* _pHead) const noexcept {
			uint8_t* pHead = reinterpret_cast<uint8_t*>(_pHead);

			_Ty* pFirst = reinterpret_cast<_Ty*>(alignUp<uint8_t>(pHead + sizeof(AllocHead), alignof(_Ty)));
			_Ty* pLast = reinterpret_cast<_Ty*>(reinterpret_cast<uint8_t*>(_pHead) + _pHead->extent);

			return { pFirst, pLast };
		}

		void reserve(size_t _Capacity) {
			if (pBase)
				VirtualFree(pBase, 0, MEM_RELEASE);

			_Capacity = _Capacity ? alignUp(_Capacity, memInfo.reservationGranularity) : memInfo.reservationGranularity;

			void* const pAlloc = VirtualAlloc(nullptr, _Capacity, MEM_RESERVE, PAGE_READWRITE);

			if (!pAlloc)
				throw std::bad_alloc();

			pBase = reinterpret_cast<uint8_t*>(pAlloc);
			pCurrent = reinterpret_cast<uint8_t*>(pAlloc);
			pEnd = reinterpret_cast<uint8_t*>(pAlloc);
			pCap = reinterpret_cast<uint8_t*>(pAlloc) + _Capacity;
		}

		void commit(size_t _Size) {
			_Size = _Size ? alignUp(_Size, memInfo.pageSize) : memInfo.pageSize;
			
			const size_t m_cap = static_cast<size_t>(pCap - pEnd);

			if (_Size > m_cap) {
				reserve(_Size);
			}

			void* const pAlloc = VirtualAlloc(pEnd, _Size, MEM_COMMIT, PAGE_READWRITE);

			if (!pAlloc)
				throw std::bad_alloc();

			pEnd += _Size;
		}

		void decommit(size_t _Size) {
			const size_t m_size = pEnd - pBase;

			if (!m_size)
				return;

			void* _pEnd = _Size < m_size ? pEnd - _Size : pEnd;
			_pEnd = alignDown<void>(_pEnd, memInfo.pageSize);

			size_t releaseSize = static_cast<size_t>(pEnd - _pEnd);

			VirtualFree(_pEnd, releaseSize, MEM_DECOMMIT);

			pEnd = reinterpret_cast<uint8_t*>(_pEnd);
		}

		void ensure(uint8_t* _pAddr) {
			if (_pAddr <= pEnd)
				return;

			if (_pAddr < pCap) {
				const size_t allocSize = alignUp(static_cast<size_t>(_pAddr - pEnd), memInfo.pageSize);

				void* pAlloc = VirtualAlloc(pEnd, allocSize, MEM_COMMIT, PAGE_READWRITE);

				if (!pAlloc)
					throw std::bad_alloc();

				pEnd += allocSize;

				return;
			}

			throw std::bad_alloc();
		}

	public:
		stack() noexcept = default;
		stack(size_t _Size) {
			reserve(_Size);

			_Size = alignUp(_Size, memInfo.pageSize);

			void* pAlloc = VirtualAlloc(pBase, _Size, MEM_COMMIT, PAGE_READWRITE);

			if (!pAlloc)
				throw std::bad_alloc();

			pCurrent = reinterpret_cast<uint8_t*>(pAlloc);
			pEnd = reinterpret_cast<uint8_t*>(pAlloc) + _Size;
		}
		stack(const stack&) = delete;
		stack(stack&& _Other) noexcept 
			:
			pBase(std::exchange(_Other.pBase, nullptr)),
			pMark(std::exchange(_Other.pMark, nullptr)),
			pCurrent(std::exchange(_Other.pCurrent, nullptr)),
			pEnd(std::exchange(_Other.pEnd, nullptr)),
			pCap(std::exchange(_Other.pCap, nullptr)),
			pOldHead(std::exchange(_Other.pOldHead, nullptr))
		{

		}

		stack& operator=(const stack&) = delete;
		stack& operator=(stack&& _Other) noexcept {
			if (this == &_Other)
				return *this;

			pBase = std::exchange(_Other.pBase, nullptr);
			pMark = std::exchange(_Other.pMark, nullptr);
			pCurrent = std::exchange(_Other.pCurrent, nullptr);
			pEnd = std::exchange(_Other.pEnd, nullptr);
			pCap = std::exchange(_Other.pCap, nullptr);
			pOldHead = std::exchange(_Other.pOldHead, nullptr);

			return *this;
		}

		~stack() noexcept {
			if (pBase)
				VirtualFree(pBase, 0, MEM_RELEASE);
		}

		size_t size() const noexcept {
			return static_cast<size_t>(pCurrent - pBase);
		}

		size_t committed() const noexcept {
			return static_cast<size_t>(pEnd - pBase);
		}

		size_t capacity() const noexcept {
			return static_cast<size_t>(pCap - pBase);
		}

		void reset() noexcept {
			if (!pBase)
				return;

			VirtualFree(pBase, pEnd - pBase, MEM_DECOMMIT);

			pMark = nullptr;
			pCurrent = pBase;
			pEnd = pBase;
		}

		void resize(size_t _Size) {
			if (pCurrent != pBase)
				throw std::logic_error("stack_not_empty!");

			const size_t m_size = static_cast<size_t>(pEnd - pBase);

			_Size = alignUp(_Size, memInfo.pageSize);
			
			if (_Size < m_size) {
				decommit(m_size - _Size);
			}
			else {
				commit(_Size - m_size);
			}

			pEnd = pBase + _Size;
		}

		template <typename _Ty>
		span<_Ty> alloc(size_t _Count) noexcept {
			if (!pBase) {
				try {
					commit(0);
				}
				catch (const std::exception& _Except) {
					return {};
				}
			}

			const uint32_t allocSize = _Count * sizeof(_Ty);

			uint8_t* pFirst = alignUp<uint8_t>(pCurrent + sizeof(AllocHead), alignof(_Ty));
			uint8_t* pLast = pFirst + allocSize;

			try {
				ensure(pLast);
			}
			catch (const std::exception& _Except) {
				return {};
			}

			AllocHead* pNewHead = locateHead(pFirst);

			*pNewHead = { static_cast<uint32_t>(pOldHead ? pNewHead - pOldHead : 0), static_cast<uint32_t>(pLast - reinterpret_cast<uint8_t*>(pNewHead))};

			pCurrent = pLast;
			pOldHead = pNewHead;

			return { reinterpret_cast<_Ty*>(pFirst), reinterpret_cast<_Ty*>(pLast) };
		}

		template <typename _Ty>
		span<_Ty> shrinkLast(_Ty* _pLast) {
			assert(pOldHead);

			span<_Ty> lastAlloc = getAlloc<_Ty>(pOldHead);

			assert(lastAlloc.contains(_pLast));

			pCurrent = reinterpret_cast<uint8_t*>(_pLast);
			pOldHead->extent = static_cast<uint32_t>(pCurrent - reinterpret_cast<uint8_t*>(pOldHead));
			
			return { lastAlloc.data(), reinterpret_cast<_Ty*>(pCurrent) };
		}

		void pop() noexcept {
			if (!pOldHead)
				return;

			pOldHead = pOldHead->prevHead ? pOldHead - pOldHead->prevHead : nullptr;
			pCurrent = pOldHead ? reinterpret_cast<uint8_t*>(pOldHead) + pOldHead->extent : pBase;
		}

		void mark() {
			if (!pBase) {
				commit(0);
			}

			if (pCurrent == pBase)
				return;

			const size_t markerOffset = pMark ? static_cast<size_t>(pMark - pBase) : 0;

			uint8_t* const pAligned = alignUp<uint8_t>(pCurrent, alignof(size_t));
			uint8_t* const pLast = pAligned + sizeof(size_t);

			ensure(pLast);

			memcpy(pAligned, &markerOffset, sizeof(size_t));

			pMark = pAligned;
			pCurrent = pLast;
		}

		void restore() noexcept {
			if (!pMark) {
				pCurrent = pBase;

				return;
			}

			size_t markerOffset;
			memcpy(&markerOffset, pMark, sizeof(size_t));

			pCurrent = pMark;

			pMark = markerOffset ? pBase + markerOffset : nullptr;
		}

		void clear() noexcept {
			pCurrent = pBase;
			pMark = nullptr;
		}

	};

	thread_local static stack scratch;

}

