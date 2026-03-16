#pragma once

namespace rhi::core {
	class device;
}

namespace rhi::primitive {
	class fence;
}

#include <vulkan/vulkan.h>

namespace rhi::procedure {

	class _watchdog {
	public:
		_watchdog(const rhi::core::device& device) noexcept;

		void watch_fence(const rhi::primitive::fence& fence) const noexcept;
		void reset_fence(const rhi::primitive::fence& fence) const noexcept;

		void wait_device() const noexcept;

	private:
		VkDevice r_device;

	};

}