#pragma once

#include <cstdint>

struct Mesh {
	uint32_t vertexOffset;
	uint32_t indexOffset;
	uint32_t vertexCount;
	uint32_t indexCount;
};