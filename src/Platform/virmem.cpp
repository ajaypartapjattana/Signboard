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

static size_t pageSize, allocationGranularity;

void virmem_init() noexcept {
#if defined(PLATFORM_WINDOWS)
	SYSTEM_INFO info{};
	GetSystemInfo(&info);

	pageSize = static_cast<size_t>(info.dwPageSize);
	allocationGranularity = static_cast<size_t>(info.dwAllocationGranularity);
#elif defined(PLATFORM_LINUX)
	size_t size = static_cast<size_t>(sysconf(_SC_PAGESIZE));

	pageSize = size;
	allocationGranularity = size;
#endif
}

void* virmem_reserve(void* allocHint, size_t* pCapacity) noexcept {
	size_t allocSize = *pCapacity;
	allocSize = allocSize ? alignUp(allocSize, allocationGranularity) : allocationGranularity;

#if defined(PLATFORM_WINDOWS)
	void* const pAlloc = VirtualAlloc(allocHint, allocSize, MEM_RESERVE, PAGE_READWRITE);
#elif defined(PLATFORM_LINUX)
	void* const _pAlloc = mmap(allocHint, allocSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	void* const pAlloc = _pAlloc != MAP_FAILED ? _pAlloc : nullptr;
#endif

	if (!pAlloc) {
		*pCapacity = 0ull;
		return nullptr;
	}

	*pCapacity = allocSize;
	return pAlloc;
}

void* virmem_commit(void* hint, size_t* pSize) noexcept {
	size_t allocSize = *pSize;
	allocSize = allocSize ? alignUp(allocSize, pageSize) : pageSize;

#if defined(PLATFORM_WINDOWS)
	void* const pAlloc = VirtualAlloc(hint, allocSize, MEM_COMMIT, PAGE_READWRITE);
#elif defined(PLATFORM_LINUX)
	int result = mprotect(hint, allocSize, PROT_READ | PROT_WRITE);
	void* const pAlloc = result == 0 ? hint : nullptr;
#endif

	if (!pAlloc) {
		*pSize = 0ull;
		return nullptr;
	}

	*pSize = allocSize;
	return pAlloc;
}

void* virmem_decommit(void* hint, size_t* pSize) noexcept {
	uint8_t* pBase = alignDown<uint8_t>(hint, pageSize);
	uint8_t* pEnd = alignUp<uint8_t>(reinterpret_cast<uint8_t*>(hint) + *pSize, pageSize);

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
	return hint;
}

void virmem_free(void* pAlloc, size_t size) noexcept {
#if defined(PLATFORM_WINDOWS)
	VirtualFree(pAlloc, 0, MEM_RELEASE);
#elif defined(PLATFORM_LINUX)
	munmap(pAlloc, alignUp(size, allocationGranularity));
#endif
}

