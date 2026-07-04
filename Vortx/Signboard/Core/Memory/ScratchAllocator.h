#pragma once

#include <stdint.h>

class ScratchAllocator {
private:
	uint8_t* memory = nullptr;
	size_t capacity = 0;
	size_t size;

public:
	ScratchAllocator() noexcept = default;
	ScratchAllocator(size_t _Size) noexcept


};