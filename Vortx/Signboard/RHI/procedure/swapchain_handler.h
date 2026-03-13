#pragma once

namespace rhi::core {
	class device;
}

namespace rhi::primitive {
	class swapchain;

	class semaphore;
}

#include <vulkan/vulkan.h>

namespace rhi::procedure{

	class swapchain_handler {
	public:
		swapchain_handler(const rhi::core::device& device) noexcept;

		swapchain_handler(const swapchain_handler&) = delete;
		swapchain_handler& operator=(const swapchain_handler&) = delete;

		void set_swapchain(const rhi::primitive::swapchain& swapchain) noexcept;

		void acquire_freeSwapchainImage(const rhi::primitive::semaphore* semaphore, uint32_t& index) const noexcept;

	private:
		VkDevice r_device = VK_NULL_HANDLE;
		VkSwapchainKHR r_swapchain = VK_NULL_HANDLE;

	};

}