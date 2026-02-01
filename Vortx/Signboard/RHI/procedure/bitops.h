#pragma once

#include <cstdint>
#include <intrin.h>

namespace rhi::procedure::bitops {

	inline uint32_t ctz(uint32_t v) {
		unsigned long idx;
		_BitScanForward(&idx, v);
		return idx;
	}

}