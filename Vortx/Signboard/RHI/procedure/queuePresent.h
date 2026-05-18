#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

#include "Signboard/Core/core.h"

namespace rhi {

	class creDevice;

	class creSwapchain;
	class pmvSemaphore;

	class pcdQueuePresent {
	public:
		pcdQueuePresent(const creDevice& device, VkPresentInfoKHR* pPresentInfo = nullptr) noexcept;

		void target_swapchains(sgb::span<const creSwapchain> swapchains) noexcept;
		void target_waitSemaphores(sgb::span<const pmvSemaphore> semaphores) noexcept;

		VkResult present(uint32_t imageIndex) noexcept;

		void preset(VkPresentInfoKHR* pPresentInfo) noexcept;
		void reset() noexcept;

	private:
		VkPresentInfoKHR* allot_basic(VkPresentInfoKHR* pCreateInfo) noexcept;

	private:
		const VkQueue r_presentQueue;

		std::vector<VkSwapchainKHR> r_swapchains;
		std::vector<VkSemaphore> waitSemaphores;

		std::unique_ptr<VkPresentInfoKHR> m_ownedInfo;
		VkPresentInfoKHR* pInfo;

	};

}