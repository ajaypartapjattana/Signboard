#pragma once

#include <vulkan/vulkan.h>

namespace rhi{
	
	class creDevice;
	
	class pmvSwapchain;
	class pmvSemaphore;
	
	class pcdSwapchainHandler {
	public:
		pcdSwapchainHandler(const rhi::creDevice& device, const rhi::pmvSwapchain& swapchain) noexcept;

		pcdSwapchainHandler(const pcdSwapchainHandler&) = delete;
		pcdSwapchainHandler& operator=(const pcdSwapchainHandler&) = delete;

		pcdSwapchainHandler(pcdSwapchainHandler&&) noexcept;
		pcdSwapchainHandler& operator=(pcdSwapchainHandler&&) noexcept;

		VkResult acquire_freeSwapchainImage(const rhi::pmvSemaphore* semaphore, uint32_t& index) const noexcept;

	private:
		VkDevice r_device;
		VkSwapchainKHR r_swapchain;

	};

}