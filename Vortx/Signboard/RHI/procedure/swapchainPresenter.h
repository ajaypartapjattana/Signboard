#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	class creDevice;

	class pmvSwapchain;
	class pmvSemaphore;

	class pcdSwapchainPresenter {
	public:
		pcdSwapchainPresenter(const rhi::creDevice& device, const rhi::pmvSwapchain& swapchain) noexcept;

		pcdSwapchainPresenter(const pcdSwapchainPresenter&) = delete;
		pcdSwapchainPresenter& operator=(const pcdSwapchainPresenter&) = delete;

		pcdSwapchainPresenter(pcdSwapchainPresenter&&) noexcept;
		pcdSwapchainPresenter& operator=(pcdSwapchainPresenter&&) noexcept;

		pcdSwapchainPresenter& update_toWait_semaphores(const rhi::pmvSemaphore* pSemaphores, uint32_t count);

		VkResult present(uint32_t imageIndex);

	private:
		VkQueue r_presentQueue;
		VkSwapchainKHR r_swapchain;

		std::vector<VkSemaphore> toWait_semaphores;

		VkPresentInfoKHR info;

	};

}