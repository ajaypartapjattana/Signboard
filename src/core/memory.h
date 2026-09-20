#pragma once

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <type_traits>

inline void* mem_allocate(const size_t _Size) noexcept {
	return std::malloc(_Size);
}

inline void mem_free(const void* const _Ptr) noexcept {
	std::free(const_cast<void*>(_Ptr));
}

inline void* mem_allocateAligned(const size_t _Size, const size_t _Alignment) noexcept {
	const size_t allocationSize = _Size + _Alignment - 1u + sizeof(void*);

	void* const raw = std::malloc(allocationSize);

	if (!raw)
		return nullptr;

	uintptr_t addr = reinterpret_cast<uintptr_t>(raw) + sizeof(void*);
	addr = (addr + _Alignment - 1) & ~(_Alignment - 1);

	void* const aligned = reinterpret_cast<void*>(addr);

	reinterpret_cast<void**>(aligned)[-1] = raw;

	return aligned;
}

inline void mem_freeAligned(const void* const _Ptr) noexcept {
	std::free(reinterpret_cast<void* const*>(const_cast<void*>(_Ptr))[-1]);
}

template <typename _Ty>
_Ty* mem_allocateRange(const size_t _Count) noexcept {
	const size_t size = sizeof(_Ty) * _Count;

	if constexpr (alignof(_Ty) > alignof(std::max_align_t))
		return reinterpret_cast<_Ty*>(mem_allocateAligned(size, alignof(_Ty)));
	else
		return reinterpret_cast<_Ty*>(mem_allocate(size));
}

template <typename _Ty, typename _Meta>
_Ty* mem_allocateMetaRange(const size_t _Count) noexcept {
	constexpr size_t align = alignof(_Ty) > alignof(_Meta) ? alignof(_Ty) : alignof(_Meta);
	constexpr size_t offset = (sizeof(_Meta) + alignof(_Ty) - 1) & ~(alignof(_Ty) - 1);

	const size_t size = offset + sizeof(_Ty) * _Count;

	void* mem;

	if constexpr (align > alignof(std::max_align_t))
		mem = mem_allocateAligned(size, align);
	else
		mem = mem_allocate(size);

	if (!mem)
		return nullptr;

	return reinterpret_cast<_Ty*>(reinterpret_cast<char*>(mem) + offset);
}

template <typename _Ty>
_Ty* mem_allocateSizeRange(const size_t _Count) noexcept {
	constexpr size_t align = alignof(_Ty) > alignof(size_t) ? alignof(_Ty) : alignof(size_t);
	constexpr size_t offset = (sizeof(size_t) + alignof(_Ty) - 1) & ~(alignof(_Ty) - 1);

	const size_t size = offset + sizeof(_Ty) * _Count;

	void* mem;

	if constexpr (align > alignof(std::max_align_t))
		mem = mem_allocateAligned(size, align);
	else
		mem = mem_allocate(size);

	if (!mem)
		return nullptr;

	*reinterpret_cast<size_t*>(mem) = _Count;

	return reinterpret_cast<_Ty*>(reinterpret_cast<char*>(mem) + offset);
}

template <typename _Ty, typename _Meta>
inline _Meta* mem_getMetaAllocationMeta(_Ty* const _Ptr) noexcept {
	constexpr size_t offset = (sizeof(_Meta) + alignof(_Ty) - 1) & ~(alignof(_Ty) - 1);

	return reinterpret_cast<_Meta*>(reinterpret_cast<char*>(_Ptr) - offset);
}

template <typename _Ty>
inline size_t mem_getSizeAllocationSize(const _Ty* const _Ptr) noexcept {
	constexpr size_t offset = (sizeof(size_t) + alignof(_Ty) - 1) & ~(alignof(_Ty) - 1);

	return *reinterpret_cast<const size_t*>(reinterpret_cast<const char*>(_Ptr) - offset);
}

template <typename _Ty>
inline const _Ty* mem_getSizeAllcoationEnd(const _Ty* const _Ptr) noexcept {
	constexpr size_t offset = (sizeof(size_t) + alignof(_Ty) - 1) & ~(alignof(_Ty) - 1);

	return _Ptr + *reinterpret_cast<const size_t*>(reinterpret_cast<const char*>(_Ptr) - offset);
}

template <typename _Ty>
inline _Ty* mem_getSizeAllcoationEnd(_Ty* const _Ptr) noexcept {
	constexpr size_t offset = (sizeof(size_t) + alignof(_Ty) - 1) & ~(alignof(_Ty) - 1);

	return _Ptr + *reinterpret_cast<size_t*>(reinterpret_cast<char*>(_Ptr) - offset);
}

template <typename _Ty>
void mem_freeRange(const _Ty* const _Ptr) noexcept {
	if constexpr (alignof(_Ty) > alignof(std::max_align_t))
		mem_freeAligned(_Ptr);
	else
		mem_free(_Ptr);
}

template <typename _Ty, typename _Meta>
void mem_freeMetaRange(const _Ty* const _Ptr) noexcept {
	constexpr size_t align = alignof(_Ty) > alignof(_Meta) ? alignof(_Ty) : alignof(_Meta);
	constexpr size_t offset = (sizeof(_Meta) + alignof(_Ty) - 1) & ~(alignof(_Ty) - 1);

	if constexpr (align > alignof(std::max_align_t))
		mem_freeAligned(reinterpret_cast<const char*>(_Ptr) - offset);
	else
		mem_free(reinterpret_cast<const char*>(_Ptr) - offset);
}

template <typename _Ty>
void mem_freeSizeRange(const _Ty* const _Ptr) noexcept {
	constexpr size_t align = alignof(_Ty) > alignof(size_t) ? alignof(_Ty) : alignof(size_t);
	constexpr size_t offset = (sizeof(size_t) + alignof(_Ty) - 1) & ~(alignof(_Ty) - 1);

	if constexpr (align > alignof(std::max_align_t))
		mem_freeAligned(reinterpret_cast<const char*>(_Ptr) - offset);
	else
		mem_free(reinterpret_cast<const char*>(_Ptr) - offset);
}

template <typename _Ty>
inline void mem_nullifyRange(_Ty* const _Ptr, const size_t _Count) noexcept {
	const size_t size = sizeof(_Ty) * _Count;

	std::memset(_Ptr, 0, size);
}

template <typename _Ty>
inline void mem_nullifySizeRange(_Ty* const _Ptr) noexcept {
	const size_t count = mem_getSizeAllocationSize(_Ptr);
	mem_nullifyRange(_Ptr, count);
}

template <typename _Ty>
inline void mem_assignRange(_Ty* const _Ptr, const size_t _Count, const _Ty& _Val) noexcept(std::is_nothrow_copy_assignable_v<_Ty>) {
	const _Ty* const pEnd = _Ptr + _Count;

	for (_Ty* pElem{ _Ptr }; pElem != pEnd; ++pElem)
		*pElem = _Val;
}

template <typename _Ty>
struct mem_span {
	_Ty* data;
	size_t count;

	_Ty* end() noexcept {
		return this->data + this->count;
	}

	const _Ty* end() const noexcept {
		return this->data + this->count;
	}
};

template <typename _Ty>
void mem_allocateSpan(mem_span<_Ty>* const pSpan, const size_t _Count) noexcept {
	_Ty* const data = mem_allocateRange<_Ty>(_Count);

	pSpan->data = data;
	pSpan->count = data ? _Count : 0u;
}

template <typename _Ty>
void mem_freeSpan(mem_span<_Ty>* const pSpan) noexcept {
	mem_freeRange(pSpan->data);
}

#include <Platform/virmem.h>
#include <core/Math/memMath.h>

class mem_stack {
private:
	uint8_t* pBase;
	uint8_t* pFrame;
	uint8_t* pCurrent;
	uint8_t* pEnd;
	uint8_t* pCap;

	struct FrameMeta {
		uint8_t* pParent;
	};

	bool ensure(const void* const _Ptr) noexcept {
		const uint8_t* const ptr8 = reinterpret_cast<const uint8_t*>(_Ptr);

		if (ptr8 <= this->pEnd)
			return 0;

		if (ptr8 > this->pCap)
			return -1;
		
		size_t commitSize = ptr8 - this->pEnd;

		void* pAlloc = virmem_commit(this->pEnd, &commitSize);

		if (!pAlloc)
			return -1;

		this->pEnd += commitSize;

		return 0;
	}

public:
	size_t capacity() const noexcept {
		return static_cast<size_t>(this->pCap - this->pBase);
	}

	int create(const size_t _Size) noexcept {
		uint8_t* _pAlloc = nullptr;
		
		do {
			size_t capacity = _Size;
			
			_pAlloc = reinterpret_cast<uint8_t*>(virmem_reserve(nullptr, &capacity));

			if (!_pAlloc)
				break;

			this->pBase = _pAlloc;
			this->pCurrent = _pAlloc;
			this->pEnd = _pAlloc;
			this->pCap = _pAlloc + capacity;
			this->pFrame = nullptr;

			if (this->ensure(_pAlloc + _Size))
				break;

			return 0;

		} while (false);

		if (_pAlloc)
			virmem_free(_pAlloc, _Size);

		this->pBase = nullptr;
		this->pEnd = nullptr;
		this->pCap = nullptr;

		return -1;
	}
	
	void reset() noexcept {
		virmem_free(this->pBase, static_cast<size_t>(this->pCap - this->pBase));
	}

	bool frame() noexcept {
		if (this->pCurrent == this->pBase)
			return true;

		const size_t parentOffset = this->pFrame ? static_cast<size_t>(this->pFrame - this->pBase) : 0u;

		uint8_t* const pAligned = alignUp<uint8_t>(this->pCurrent, alignof(FrameMeta));
		uint8_t* const pAllocEnd = pAligned + sizeof(FrameMeta);

		if (this->ensure(pAllocEnd))
			return false;

		memcpy(pAligned, &parentOffset, sizeof(FrameMeta));

		this->pFrame = pAligned;
		this->pCurrent = pAllocEnd;

		return true;
	}

	void restore() noexcept {
		FrameMeta* pFrame = reinterpret_cast<FrameMeta*>(this->pFrame);

		if (!pFrame) {
			this->pCurrent = this->pBase;

			return;
		}

		FrameMeta meta;
		memcpy(&meta, pFrame, sizeof(FrameMeta));

		this->pCurrent = this->pFrame;

		this->pFrame = meta.pParent ? meta.pParent : nullptr;
	}
	
	void* alloc(const size_t _Size, const size_t _Align) noexcept {
		if (!_Size)
			return nullptr;

		uint8_t* pBegin = alignUp<uint8_t>(this->pCurrent, _Align);
		uint8_t* pAllocEnd = pBegin + _Size;

		if (this->ensure(pAllocEnd))
			return nullptr;

		this->pCurrent = pAllocEnd;

		return pBegin;
	}

	void* realloc(void* const pAlloc, const size_t _NewSize) noexcept {
		void* const pAllocEnd = reinterpret_cast<uint8_t*>(pAlloc) + _NewSize;

		if (this->ensure(pAllocEnd))
			return nullptr;

		this->pCurrent = reinterpret_cast<uint8_t*>(pAllocEnd);

		return pAlloc;
	}
};

template <typename _Ty>
_Ty* mem_stackAllocateRange(mem_stack* const pStack, const size_t _Count) noexcept {
	const size_t allocSize = sizeof(_Ty) * _Count;

	return reinterpret_cast<_Ty*>(pStack->alloc(allocSize, alignof(_Ty)));
}

template <typename _Ty>
void mem_stackAllocateSpan(mem_stack* const pStack, mem_span<_Ty>* const pSpan, const size_t _Count) noexcept {
	const size_t allocSize = sizeof(_Ty) * _Count;

	_Ty* const pAlloc = reinterpret_cast<_Ty*>(pStack->alloc(allocSize, alignof(_Ty)));

	pSpan->data = pAlloc;
	pSpan->count = pAlloc ? _Count : 0u;
}

template <typename _Ty>
struct mem_smallset {
	_Ty* data;
	_Ty* pCurrent;

	bool empty() const noexcept {
		return pCurrent == data;
	}

	size_t size() const noexcept {
		return static_cast<size_t>(this->pCurrent - this->data);
	}

	void push(const _Ty& _Val) noexcept {
		for (const _Ty* pVal{ this->data }; pVal != this->pCurrent; ++pVal) {
			if (*pVal == _Val)
				return;
		}

		*(pCurrent++) = _Val;
	}
};

template <typename _Ty>
void mem_stackAllocateSmallSet(mem_stack* const pStack, mem_smallset<_Ty>* const pSmallset, const size_t _Count) noexcept {
	const size_t size = sizeof(_Ty) * _Count;

	_Ty* const pAlloc = static_cast<_Ty*>(pStack->alloc(size, alignof(_Ty)));

	pSmallset->data = pAlloc;
	pSmallset->pCurrent = pAlloc;
}

#if 0

#include <cstring>
#include <utility>
#include <cassert>
#include <stdexcept>
#include <type_traits>

namespace mem {

	template <typename _Ty>
	struct span {
		_Ty* pBegin = nullptr;
		_Ty* pEnd = nullptr;

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

		void copy(const span<_Ty>& _Other) noexcept {
			assert(_Other.size() == size());

			memcpy(pBegin, _Other.pBegin, size() * sizeof(_Ty));
		}

		const _Ty& back() const noexcept {
			return *(pEnd - 1u);
		}

		_Ty& back() noexcept {
			return *(pEnd - 1u);
		}

		_Ty& operator[](size_t _Index) noexcept {
			return pBegin[_Index];
		}

		const _Ty& operator[](size_t _Index) const noexcept {
			return pBegin[_Index];
		}

		size_t size() const noexcept {
			return static_cast<size_t>(pEnd - pBegin);
		}

		size_t index(_Ty* const _Addr) const noexcept {
			return static_cast<size_t>(_Addr - pBegin);
		}

		explicit operator bool() const noexcept {
			return pBegin && pEnd;
		}

		operator _Ty* () const noexcept {
			return pBegin;
		}

		bool empty() const noexcept {
			return pBegin == pEnd;
		}

		bool contains(const span& _Other) const noexcept {
			return _Other.pBegin >= pBegin && _Other.pEnd <= pEnd;
		}

		bool overlaps(const span& _Other) const noexcept {
			return pBegin < _Other.pEnd && _Other.pBegin < pEnd;
		}

		void remove_prefix(size_t _Count) noexcept {
			assert(_Count <= size());

			pBegin += _Count;
		}

		void remove_suffix(size_t _Count) noexcept {
			assert(_Count <= size());

			pEnd -= _Count;
		}

		void assign(const _Ty& _Val) const noexcept(std::is_nothrow_copy_assignable_v<_Ty>) {
			std::fill(pBegin, pEnd, _Val);
		}

		void assign_default() const noexcept {
			if constexpr (std::is_trivially_constructible_v<_Ty> && std::is_trivially_destructible_v<_Ty>) {
				memset(pBegin, 0, static_cast<size_t>(pEnd - pBegin) * sizeof(_Ty));
			}
			else {
				for (_Ty* ptr{ pBegin }; ptr != pEnd; ++ptr)
					::new (ptr) _Ty{};
			}
		}

		bool adjacent(const span& _Other) const noexcept {
			return pEnd == _Other.pBegin || pBegin == _Other.pEnd;
		}

		span slice(size_t _Size) noexcept {
			_Ty* _pSliceEnd = pBegin + _Size;
			_pSliceEnd = _pSliceEnd < pEnd ? _pSliceEnd : pEnd;

			span span{ pBegin, _pSliceEnd };
			pBegin = _pSliceEnd;

			return span;
		}

		void merge(const span& _Other) noexcept {
			assert(adjacent(_Other) || overlaps(_Other));

			pBegin = pBegin < _Other.pBegin ? pBegin : _Other.pBegin;
			pEnd = pEnd > _Other.pEnd ? pEnd : _Other.pEnd;
		}

	};

	template <typename _Ty>
	span<_Ty> allocate_range(const size_t _Count) noexcept {
		if (_Count == 0)
			return span<_Ty>{};

		const size_t size = sizeof(_Ty) * _Count;

		void* memory = ::operator new(size, std::nothrow);

		if (!memory)
			return span<_Ty>{};

		 _Ty* const pBegin = static_cast<_Ty*>(memory);

    	return { pBegin, pBegin + _Count };
	}

	template <typename _Ty>
	void free_range(const span<_Ty>& _Span) noexcept {
		::operator delete(_Span.pBegin);
	}

	template <typename _Ty>
	struct static_vector {
		_Ty* pBegin = nullptr;
		_Ty* pEnd = nullptr;
		_Ty* pCurrent = nullptr;

		static_vector() noexcept = default;
		static_vector(const span<_Ty>& _Span) noexcept 
			: pBegin(_Span.pBegin)
			, pEnd(_Span.pEnd)
			, pCurrent(_Span.pBegin)
		{

		}

		span<_Ty> memory() noexcept {
			return span<_Ty>{ pBegin, pEnd };
		}

		static_vector& operator=(const span<_Ty>& _Span) noexcept {
			pBegin = _Span.pBegin;
			pEnd = _Span.pEnd;
			pCurrent = _Span.pBegin;

			return *this;
		}

		_Ty& operator[](size_t _Index) noexcept {
			return pBegin[_Index];
		}

		const _Ty& operator[](size_t _Index) const noexcept {
			return pBegin[_Index];
		}

		explicit operator bool() const noexcept {
			return pBegin && pEnd;
		}

		span<_Ty> view() const noexcept {
			return span<_Ty>{ pBegin, pCurrent };
		}

		size_t size() const noexcept {
			return static_cast<size_t>(pCurrent - pBegin);
		}

		size_t capacity() const noexcept {
			return static_cast<size_t>(pEnd - pBegin);
		}

		bool empty() const noexcept {
			return pCurrent == pBegin;
		}

		void clear() noexcept {
			if constexpr (!std::is_trivially_destructible_v<_Ty>)
				while (pCurrent != pBegin)
					(--pCurrent)->~_Ty();
			else
				pCurrent = pBegin;
		}

		void assign_default() noexcept {
			if constexpr (std::is_trivially_constructible_v<_Ty> && std::is_trivially_destructible_v<_Ty>) {
				memset(pBegin, 0, static_cast<size_t>(pEnd - pBegin) * sizeof(_Ty));
			}
			else {
				for (_Ty* ptr{ pBegin }; ptr != pEnd; ++ptr)
					::new (ptr) _Ty{};
			}
		}

		void push_back_unique(_Ty& _Val) noexcept {
			for (const _Ty* pVal{ pBegin }; pVal != pCurrent; ++pVal) {
				if (*pVal == _Val)
					return;
			}

			*(pCurrent++) = _Val;
		}

		void push_back(_Ty& _Val) noexcept {
			*(pCurrent++) = _Val;
		}

		void push_back_unique(_Ty&& _Val) noexcept {
			for (const _Ty* pVal{ pBegin }; pVal != pCurrent; ++pVal) {
				if (*pVal == _Val)
					return;
			}

			*(pCurrent++) = std::move(_Val);
		}

		void push_back(_Ty&& _Val) noexcept {
			*(pCurrent++) = std::move(_Val);
		}

		template <class... Args>
		_Ty* emplace_back(Args&&... args) noexcept {
			::new (pCurrent) _Ty(std::forward<Args>(args)...);

			return pCurrent++;
		}

		void pop_back() noexcept {
			--pCurrent;

			if constexpr (!std::is_trivially_destructible_v<_Ty>)
				pCurrent->~Ty();
		}

	};

	class virtualBlock {
	protected:
		struct SystemMemoryInfo {
			size_t pageSize;
			size_t allocationGranularity;
		};

	#if defined(PLATFORM_WINDOWS)
		inline static const SystemMemoryInfo memInfo = [] {
			SYSTEM_INFO info;
			GetSystemInfo(&info);
			
			return SystemMemoryInfo{
				static_cast<size_t>(info.dwPageSize),
				static_cast<size_t(info.dwAllocationGranularity)
			};
		}();
	#elif defined(PLATFORM_LINUX)
		inline static const SystemMemoryInfo memInfo = [] {
			const size_t pageSize = static_cast<size_t>(sysconf(_SC_PAGESIZE));

			return SystemMemoryInfo{
				pageSize,
				pageSize
			};
		}();
	#endif

		void* reserve(void* _AllocHint, size_t* pCapacity) const noexcept {
			assert(pCapacity);

			size_t allocSize = *pCapacity;
			allocSize = allocSize ? alignUp(allocSize, memInfo.allocationGranularity) : memInfo.allocationGranularity;

		#if defined(PLATFORM_WINDOWS)
			void* const pAlloc = VirtualAlloc(_AllocHint, allocSize, MEM_RESERVE, PAGE_READWRITE);
		#elif defined(PLATFORM_LINUX)
			void* const _pAlloc = mmap(_AllocHint, allocSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
			void* const pAlloc = _pAlloc != MAP_FAILED ? _pAlloc : nullptr;
		#endif

			if (!pAlloc) {
				*pCapacity = 0ull;
				return nullptr;
			}

			*pCapacity = allocSize;
			return pAlloc;
		}

		void* commit(void* _Hint, size_t* pSize) const noexcept {
			assert(_Hint && pSize);

			size_t allocSize = *pSize;
			allocSize = allocSize ? alignUp(allocSize, memInfo.pageSize) : memInfo.pageSize;

		#if defined(PLATFORM_WINDOWS)
			void* const pAlloc = VirtualAlloc(_Hint, allocSize, MEM_COMMIT, PAGE_READWRITE);
		#elif defined(PLATFORM_LINUX)
			int result = mprotect(_Hint, allocSize, PROT_READ | PROT_WRITE);
			void* const pAlloc = result == 0 ? _Hint : nullptr;
		#endif

			if (!pAlloc) {
				*pSize = 0ull;
				return nullptr;
			}

			*pSize = allocSize;
			return pAlloc;
		}

		void* decommit(void* _Hint, size_t* pSize) const noexcept {
			assert(_Hint && pSize);

			uint8_t* pBase = alignDown<uint8_t>(_Hint, memInfo.pageSize);
			uint8_t* pEnd = alignUp<uint8_t>(reinterpret_cast<uint8_t*>(_Hint) + *pSize, memInfo.pageSize);

			const size_t releaseSize = static_cast<size_t>(pEnd - pBase);

			bool success = false;

		#if defined(PLATFORM_WINDOWS)
			success = VirtualFree((void*)pBase, releaseSize, MEM_DECOMMIT) != FALSE;
		#elif defined(PLATFORM_LINUX)
			success = madvise((void*)pBase, releaseSize, MADV_DONTNEED) == 0;
		#endif
			if (!success) {
				*pSize = 0;
				return nullptr;
			}

			*pSize = releaseSize;
			return _Hint;
		}

		void free(void* const pAlloc, size_t _Size) const noexcept {
			assert(pAlloc);

		#if defined(PLATFORM_WINDOWS)
			VirtualFree(pAlloc, 0, MEM_RELEASE);
		#elif defined(PLATFORM_LINUX)
			_Size = alignUp(_Size, memInfo.allocationGranularity);
			munmap(pAlloc, _Size);
		#endif
		}

	};

	struct marker {
		uint8_t* mark = nullptr;

		bool marked() const noexcept {
			return mark;
		}
	};

	class stack : virtualBlock {
	private:
		uint8_t* pBase = nullptr;
		uint8_t* pCurrent = nullptr;
		uint8_t* pEnd = nullptr;
		uint8_t* pCap = nullptr;
		
		struct Marker {
			size_t prevMark;
		};
		
		uint8_t* pMark = nullptr;

		void ensure(uint8_t* const _Addr) {
			if (_Addr <= pEnd)
				return;

			if (_Addr <= pCap) {
				size_t commitSize = _Addr - pEnd;

				void* pAlloc = commit(pEnd, &commitSize);

				if (!pAlloc)
					throw std::bad_alloc();

				pEnd += commitSize;

				return;
			}

			throw std::bad_alloc();
		}

	public:

		stack() noexcept = default;
		stack(size_t _Size) {
			size_t allocSize = _Size;

			void* pAlloc = reserve(nullptr, &allocSize);

			if (!pAlloc)
				throw std::bad_alloc();

			pBase = reinterpret_cast<uint8_t*>(pAlloc);
			pCurrent = reinterpret_cast<uint8_t*>(pAlloc);
			pCap = reinterpret_cast<uint8_t*>(pAlloc) + allocSize;

			pAlloc = commit(pBase, &_Size);

			if (!pAlloc)
				throw std::bad_alloc();

			pEnd = reinterpret_cast<uint8_t*>(pAlloc) + _Size;
		}
		stack(const stack&) = delete;
		stack(stack&& _Other) noexcept
			:
			pBase(std::exchange(_Other.pBase, nullptr)),
			pCurrent(std::exchange(_Other.pCurrent, nullptr)),
			pEnd(std::exchange(_Other.pEnd, nullptr)),
			pCap(std::exchange(_Other.pCap, nullptr)),
			pMark(std::exchange(_Other.pMark, nullptr))
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

			return *this;
		}

		~stack() noexcept {
			if (!pBase)
				return;

			free(pBase, static_cast<size_t>(pCap - pBase));
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

		void resize(size_t _Size) {
			if (!pCap) {
				size_t capacity = _Size;

				void* pAlloc = reserve(nullptr, &capacity);

				if (!pAlloc)
					throw std::bad_alloc();

				pBase = reinterpret_cast<uint8_t*>(pAlloc);
				pCurrent = reinterpret_cast<uint8_t*>(pAlloc);
				pEnd = reinterpret_cast<uint8_t*>(pAlloc);
				pCap = reinterpret_cast<uint8_t*>(pAlloc) + capacity;
			}

			uint8_t* const pReq = pBase + _Size;

			ensure(pReq);
		}

		void reset() noexcept {
			if (!pBase)
				return;

			free(pBase, static_cast<size_t>(pCap - pBase));

			pBase = nullptr;
			pCurrent = nullptr;
			pEnd = nullptr;
			pCap = nullptr;

			pMark = nullptr;
		}

		void clear() noexcept {
			pCurrent = pBase;
			pMark = nullptr;
		}

		marker mark() noexcept {
			if (!pBase) {
				try {
					resize(0);
				}
				catch (const std::exception& _Except) {
					return { nullptr };
				}
			}

			if (pCurrent == pBase)
				return { pBase };

			const size_t markerOffset = pMark ? static_cast<size_t>(pMark - pBase) : 0;

			uint8_t* const pAligned = alignUp<uint8_t>(pCurrent, alignof(Marker));
			uint8_t* const pLast = pAligned + sizeof(Marker);

			try {
				ensure(pLast);
			}
			catch (const std::exception& _Except) {
				return { nullptr };
			}

			memcpy(pAligned, &markerOffset, sizeof(Marker));

			pMark = pAligned;
			pCurrent = pLast;

			return { pAligned };
		}

		void restore(marker _Marker = {}) noexcept {
			pMark = _Marker.mark ? _Marker.mark : pMark;

			if (!pMark) {
				pCurrent = pBase;

				return;
			}

			size_t markerOffset;
			memcpy(&markerOffset, pMark, sizeof(Marker));

			pCurrent = pMark;

			pMark = markerOffset ? pBase + markerOffset : nullptr;
		}

		template <typename _Ty>
		_Ty* alloc() noexcept {
			if (!pBase) {
				try {
					resize(sizeof(_Ty));
				}
				catch (const std::exception& _Except) {
					return nullptr;
				}
			}

			uint8_t* pThis = alignUp<uint8_t>(pCurrent, alignof(_Ty));
			uint8_t* pTill = pThis + sizeof(_Ty);

			try {
				ensure(pTill);
			}
			catch (const std::exception& _Except) {
				return nullptr;
			}

			pCurrent = pTill;

			return pThis;
		}

		template <typename _Ty>
		span<_Ty> alloc(size_t _Count) noexcept {
			if (!_Count)
				return {};

			const size_t allocSize = _Count * sizeof(_Ty);

			if (!pBase) {
				try {
					resize(allocSize);
				}
				catch (const std::exception& _Except) {
					return {};
				}
			}

			uint8_t* pFirst = alignUp<uint8_t>(pCurrent, alignof(_Ty));
			uint8_t* pLast = pFirst + allocSize;

			try {
				ensure(pLast);
			}
			catch (const std::exception& _Except) {
				return{};
			}

			pCurrent = pLast;

			return { reinterpret_cast<_Ty*>(pFirst), reinterpret_cast<_Ty*>(pLast) };
		}

		template <typename _Ty>
		span<_Ty> realloc(const span<_Ty>& _Last, size_t _NewCount) noexcept {
			_Ty* pFirst = _Last.begin();
			_Ty* pLast = pFirst + _NewCount;

			try {
				ensure(reinterpret_cast<uint8_t*>(pLast));
			}
			catch (const std::exception& _Except) {
				return {};
			}

			pCurrent = reinterpret_cast<uint8_t*>(pLast);

			return { pFirst, pLast };
		}

	};

	class smartStack : virtualBlock {
	private:
		uint8_t* pBase = nullptr;
		uint8_t* pCurrent = nullptr;
		uint8_t* pEnd = nullptr;
		uint8_t* pCap = nullptr;

		struct AllocHead {
			uint32_t prevHead;
			uint32_t extent;
		};

		AllocHead* pOldHead = nullptr;

		template <typename _Ty>
		AllocHead* locateHead(_Ty* pAlloc) const noexcept {
			return alignDown<AllocHead>(reinterpret_cast<uint8_t*>(pAlloc) - sizeof(AllocHead), sizeof(AllocHead));
		}

		template <typename _Ty>
		span<_Ty> getAlloc(AllocHead* _pHead) const noexcept {
			uint8_t* pHead = reinterpret_cast<uint8_t*>(_pHead);

			_Ty* pFirst = reinterpret_cast<_Ty*>(alignUp<uint8_t>(pHead + sizeof(AllocHead), alignof(_Ty)));
			_Ty* pLast = reinterpret_cast<_Ty*>(reinterpret_cast<uint8_t*>(_pHead) + _pHead->extent);

			return { pFirst, pLast };
		}

		void ensure(uint8_t* const _Addr) {
			if (_Addr <= pEnd)
				return;

			if (_Addr < pCap) {
				size_t commitSize = _Addr - pEnd;

				void* pAlloc = commit(pEnd, &commitSize);

				if (!pAlloc)
					throw std::bad_alloc();

				pEnd += commitSize;

				return;
			}

			throw std::bad_alloc();
		}

	public:
		smartStack() noexcept = default;
		smartStack(size_t _Size) {
			size_t allocSize = _Size;

			void* pAlloc = reserve(nullptr, &allocSize);

			if (!pAlloc)
				throw std::bad_alloc();

			pBase = reinterpret_cast<uint8_t*>(pAlloc);
			pCurrent = reinterpret_cast<uint8_t*>(pAlloc);
			pCap = reinterpret_cast<uint8_t*>(pAlloc) + allocSize;

			pAlloc = commit(pBase, &_Size);

			if (!pAlloc)
				throw std::bad_alloc();

			pEnd = reinterpret_cast<uint8_t*>(pAlloc) + _Size;
		}
		smartStack(const smartStack&) = delete;
		smartStack(smartStack&& _Other) noexcept
			:
			pBase(std::exchange(_Other.pBase, nullptr)),
			pCurrent(std::exchange(_Other.pCurrent, nullptr)),
			pEnd(std::exchange(_Other.pEnd, nullptr)),
			pCap(std::exchange(_Other.pCap, nullptr)),
			pOldHead(std::exchange(_Other.pOldHead, nullptr))
		{

		}

		smartStack& operator=(const smartStack&) = delete;
		smartStack& operator=(smartStack&& _Other) noexcept {
			if (this == &_Other)
				return *this;

			pBase = std::exchange(_Other.pBase, nullptr);
			pCurrent = std::exchange(_Other.pCurrent, nullptr);
			pEnd = std::exchange(_Other.pEnd, nullptr);
			pCap = std::exchange(_Other.pCap, nullptr);
			pOldHead = std::exchange(_Other.pOldHead, nullptr);

			return *this;
		}

		~smartStack() noexcept {
			if (!pBase)
				return;

			free(pBase, static_cast<size_t>(pCap - pBase));
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

			free(pBase, static_cast<size_t>(pCap - pBase));

			pBase = nullptr;
			pCurrent = nullptr;
			pEnd = nullptr;
			pCap = nullptr;

			pOldHead = nullptr;
		}

		void resize(size_t _Size) {
			if (!pCap) {
				size_t capacity = _Size;

				void* pAlloc = reserve(nullptr, &capacity);

				if (!pAlloc)
					throw std::bad_alloc();

				pBase = reinterpret_cast<uint8_t*>(pAlloc);
				pCurrent = reinterpret_cast<uint8_t*>(pAlloc);
				pEnd = reinterpret_cast<uint8_t*>(pAlloc);
				pCap = reinterpret_cast<uint8_t*>(pAlloc) + capacity;
			}

			uint8_t* const pReq = pBase + _Size;

			ensure(pReq);
		}

		template <typename _Ty>
		span<_Ty> alloc(size_t _Count) noexcept {
			if (!pBase) {
				try {
					resize(0);
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
				return{};
			}

			AllocHead* pNewHead = locateHead(pFirst);

			*pNewHead = { static_cast<uint32_t>(pOldHead ? pNewHead - pOldHead : 0), static_cast<uint32_t>(pLast - reinterpret_cast<uint8_t*>(pNewHead)) };

			pCurrent = pLast;
			pOldHead = pNewHead;

			return { reinterpret_cast<_Ty*>(pFirst), reinterpret_cast<_Ty*>(pLast) };
		}

		template <typename _Ty>
		void realloc(size_t _Count, span<_Ty>& _Span) noexcept {
			assert(_Span && pOldHead);

			span<_Ty> lastAlloc = getAlloc<_Ty>(pOldHead);

			assert(_Span.data() == lastAlloc.data());

			uint8_t* const pLast = reinterpret_cast<uint8_t*>(_Span.data() + _Count);

			try {
				ensure(pLast);
			}
			catch (const std::exception& _Except) {
				return;
			}

			pCurrent = pLast;
			pOldHead->extent = static_cast<uint32_t>(pCurrent - reinterpret_cast<uint8_t*>(pOldHead));

			_Span = { _Span.data(), reinterpret_cast<_Ty*>(pCurrent) };
		}

		template <typename _Ty>
		void realloc(_Ty* _pLast, span<_Ty> _Span) noexcept {
			assert(_Span && pOldHead);

			span<_Ty> lastAlloc = getAlloc<_Ty>(pOldHead);

			assert(_Span.data() == lastAlloc.data() && _pLast >= _Span.data());

			uint8_t* const pLast = reinterpret_cast<uint8_t*>(_pLast);

			try {
				ensure(pLast);
			}
			catch (const std::exception& _Except) {
				return;
			}

			pCurrent = pLast;
			pOldHead->extent = static_cast<uint32_t>(pCurrent - reinterpret_cast<uint8_t*>(pOldHead));

			_Span = { _Span.data(), reinterpret_cast<_Ty*>(pCurrent) };
		}

		void pop() noexcept {
			assert(pOldHead);

			pOldHead = pOldHead->prevHead ? pOldHead - pOldHead->prevHead : nullptr;
			pCurrent = pOldHead ? reinterpret_cast<uint8_t*>(pOldHead) + pOldHead->extent : pBase;
		}

		void clear() noexcept {
			pCurrent = pBase;
			pOldHead = nullptr;
		}

	};

}
#endif