#pragma once

namespace rhi::core {
	class device;
}

namespace rhi::primitive {
	class swapchain;
	class semaphore;
}

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::procedure {

	class swapchain_presenter {
	public:
		swapchain_presenter(const rhi::core::device& device, const rhi::primitive::swapchain& swapchain) noexcept;

		swapchain_presenter(const swapchain_presenter&) = delete;
		swapchain_presenter& operator=(const swapchain_presenter&) = delete;

		swapchain_presenter(swapchain_presenter&&) noexcept;
		swapchain_presenter& operator=(swapchain_presenter&&) noexcept;

		swapchain_presenter& update_toWait_semaphores(const rhi::primitive::semaphore* pSemaphores, uint32_t count);

		VkResult present(uint32_t imageIndex);

	private:
		VkQueue r_presentQueue = VK_NULL_HANDLE;
		VkSwapchainKHR r_swapchain = VK_NULL_HANDLE;

		std::vector<VkSemaphore> toWait_semaphores;

		VkPresentInfoKHR info{};

	};

}