#pragma once

#include "Signboard/RHI/primitive/commandPool.h"

namespace rhi {
	struct standardCommandPools {
		rhi::pmvCommandPool graphicsPool;
		rhi::pmvCommandPool computePool;
		rhi::pmvCommandPool transferPool;
		rhi::pmvCommandPool presentPool;
	};
}
