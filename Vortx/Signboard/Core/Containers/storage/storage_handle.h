#pragma once

#include <cstdint>

namespace storage {

	struct storage_handle {
		uint32_t index;
		uint32_t generation;
	};

}