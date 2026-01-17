#pragma once

#include <cstdint>

namespace storage {

	struct Handle {
		uint32_t index;
		uint32_t generation;
	};

}