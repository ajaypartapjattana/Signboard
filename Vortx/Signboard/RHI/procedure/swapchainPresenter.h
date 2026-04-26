#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	class creDevice;

	class pmvSwapchain;
	class pmvSemaphore;

	class pcdQueuePresent {
	public:
		pcdQueuePresent(const creDevice& device, const VkPresentInfoKHR* pCreateInfo = nullptr) noexcept;

		pcdQueuePresent(const pcdQueuePresent&) = delete;
		pcdQueuePresent& operator=(const pcdQueuePresent&) = delete;

		pcdQueuePresent(pcdQueuePresent&&) noexcept;
		pcdQueuePresent& operator=(pcdQueuePresent&&) = delete;

		void target_swapchain(const pmvSwapchain& swapchain) noexcept;

		pcdQueuePresent& add_wait(const pmvSemaphore* pSemaphores, uint32_t count);

		VkResult present(uint32_t imageIndex);

	private:
		VkPresentInfoKHR fetch_basic(const VkPresentInfoKHR* pCreateInfo) const noexcept;

	private:
		const VkQueue r_presentQueue;

		VkSwapchainKHR r_swapchain;
		std::vector<VkSemaphore> waitSemaphores;

		VkPresentInfoKHR _info;

	};

}