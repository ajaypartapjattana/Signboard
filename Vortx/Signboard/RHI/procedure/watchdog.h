#pragma once

#include <vulkan/vulkan.h>

namespace rhi {
	
	class creDevice;

	class pmvFence;

	class pcdWatchdog {
	public:
		pcdWatchdog(const creDevice& device) noexcept;

		void watch_fence(const pmvFence& fence) const noexcept;
		void reset_fence(const pmvFence& fence) const noexcept;

		void wait_device() const noexcept;

	private:
		const VkDevice r_device;

	};

}