#pragma once

namespace rhi::core {
	class device;
}

namespace rhi::primitive {
	class fence;
}

#include <vulkan/vulkan.h>

namespace rhi::procedure {

	class fence_watchdog {
	public:
		fence_watchdog(const rhi::core::device& device) noexcept;

		void watch(const rhi::primitive::fence& fence);

	private:
		VkDevice r_device;

	};

}