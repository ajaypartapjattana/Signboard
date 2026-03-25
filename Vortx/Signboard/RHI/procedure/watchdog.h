#pragma once

#include <vulkan/vulkan.h>

namespace rhi {
	
	class creDevice;

	class pmvFence;

	class pcdWatchdog {
	public:
		pcdWatchdog(const rhi::creDevice& device) noexcept;

		void watch_fence(const rhi::pmvFence& fence) const noexcept;
		void reset_fence(const rhi::pmvFence& fence) const noexcept;

		void wait_device() const noexcept;

	private:
		VkDevice r_device;

	};

}