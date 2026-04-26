#pragma once

#include <vulkan/vulkan.h>

namespace rhi{
	
	class creDevice;
	
	class pmvSwapchain;
	class pmvSemaphore;
	
	class pcdSwapchainImageAcquire {
	public:
		pcdSwapchainImageAcquire(const creDevice& device) noexcept;

		pcdSwapchainImageAcquire(const pcdSwapchainImageAcquire&) = delete;
		pcdSwapchainImageAcquire& operator=(const pcdSwapchainImageAcquire&) = delete;

		pcdSwapchainImageAcquire(pcdSwapchainImageAcquire&&) noexcept;
		pcdSwapchainImageAcquire& operator=(pcdSwapchainImageAcquire&&) = delete;

		void target_swapchain(const pmvSwapchain& swapchain) noexcept;

		VkResult acquire_freeSwapchainImage(const pmvSemaphore* semaphore, uint32_t& index) const noexcept;

	private:
		const VkDevice r_device;

		VkSwapchainKHR r_swapchain;

	};

}