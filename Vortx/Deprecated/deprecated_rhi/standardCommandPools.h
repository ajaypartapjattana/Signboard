#pragma once

#include "Signboard/RHI/primitive/commandPool.h"

namespace rhi {
	struct stdCommandPools {
		pmvCommandPool graphicsPool;
		pmvCommandPool computePool;
		pmvCommandPool transferPool;
		pmvCommandPool presentPool;
	};
}
