#pragma once

#include <cstdint>

void* virmem_reserve(void* _AllocHint, size_t* pCapacity) noexcept;
void* virmem_commit(void* _Hint, size_t* pSize) noexcept;
void* virmem_decommit(void* _Hint, size_t* pSize) noexcept;
void virmem_free(void* pAlloc, size_t _Size) noexcept;