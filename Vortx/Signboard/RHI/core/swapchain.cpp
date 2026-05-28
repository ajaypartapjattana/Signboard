#include "swapchain.hh"

namespace rhi {

	VkResult swapchain::create(VkDevice device, VkSwapchainCreateInfoKHR* pCreateInfo) noexcept {
		VkSwapchainKHR swapchain;
		VkResult result = vkCreateSwapchainKHR(device, pCreateInfo, nullptr, &swapchain);
		if (result != VK_SUCCESS)
			return result;

		r_device = device;
		m_swapchain = swapchain;

		return VK_SUCCESS;
	}

	void swapchain::reset() noexcept {
		if (r_device) {
			vkDestroySwapchainKHR(r_device, m_swapchain, nullptr);
			r_device = VK_NULL_HANDLE;
		}

		m_swapchain = VK_NULL_HANDLE;
	}

}