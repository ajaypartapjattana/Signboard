#pragma once

#include <cstdint>

void virmem_init() noexcept;
void* virmem_reserve(void* allocHint, size_t* pCapacity) noexcept;
void* virmem_commit(void* hint, size_t* pSize) noexcept;
void* virmem_decommit(void* hint, size_t* pSize) noexcept;
void virmem_free(void* pAlloc, size_t size) noexcept;
