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
