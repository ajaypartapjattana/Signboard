#pragma once

namespace rhi::core {
	class device;
}

#include "Signboard/RHI/primitive/commandPool.h"

#include <vector>
#include <set>

struct std_commandPools {
	rhi::primitive::commandPool graphicsPool;
	rhi::primitive::commandPool computePool;
	rhi::primitive::commandPool transferPool;
	rhi::primitive::commandPool presentPool;
};

namespace rhi::procedure {

	class commandPool_creator {
	public:
		commandPool_creator(const rhi::core::device& device);

		VkResult create(std_commandPools& commandPools);

	private:
		const rhi::core::device& m_device;

	};

}
