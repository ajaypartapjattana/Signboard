#include "memory.h"

#include <core/Math/memMath.h>

#if defined(PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>

#elif defined(PLATFORM_LINUX)

#include <unistd.h>
#include <sys/mman.h>

#else

#error "Unsupported platform"

#endif

static size_t virmem_pageSize() noexcept {
	return static_cast<size_t>(sysconf(_SC_PAGESIZE));
}

static size_t virmem_allocationGranularity() noexcept {
	return static_cast<size_t>(sysconf(_SC_PAGESIZE));
}

void* virmem_reserve(void* const _AllocHint, size_t* const pCapacity) noexcept {
	const size_t allocGran = virmem_allocationGranularity();
	
	size_t allocSize = *pCapacity;
	allocSize = allocSize ? alignUp(allocSize, allocGran) :allocGran;

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

void* virmem_commit(void* const _Hint, size_t* const pSize) noexcept {
	const size_t pageSize = virmem_pageSize();
	
	size_t allocSize = *pSize;
	allocSize = allocSize ? alignUp(allocSize, pageSize) : pageSize;

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

void* virmem_decommit(void* const _Hint, size_t* const pSize) noexcept {
	const size_t pageSize = virmem_pageSize();
	
	uint8_t* pBase = alignDown<uint8_t>(_Hint, pageSize);
	uint8_t* pEnd = alignUp<uint8_t>(reinterpret_cast<uint8_t*>(_Hint) + *pSize, pageSize);

	const size_t releaseSize = static_cast<size_t>(pEnd - pBase);

	int result;

#if defined(PLATFORM_WINDOWS)
	result = VirtualFree((void*)pBase, releaseSize, MEM_DECOMMIT) != FALSE;
#elif defined(PLATFORM_LINUX)
	result = madvise((void*)pBase, releaseSize, MADV_DONTNEED) == 0;
#endif

	if (!result) {
		*pSize = 0;
		return nullptr;
	}

	*pSize = releaseSize;
	return _Hint;
}

void virmem_free(void* const pAlloc, const size_t _Size) noexcept {
#if defined(PLATFORM_WINDOWS)
	VirtualFree(pAlloc, 0, MEM_RELEASE);
#elif defined(PLATFORM_LINUX)
	const size_t allocGran = virmem_allocationGranularity();
	munmap(pAlloc, alignUp(_Size, allocGran));
#endif
}

