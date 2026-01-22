#include "swapchain.h"

namespace rhi::core {

	swapchain::swapchain(swapchain&& other) noexcept {
		m_swapchain = other.m_swapchain;
		m_allocator = other.m_allocator;
		m_device = other.m_device;

		other.m_swapchain = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
		other.m_allocator = nullptr;
	}

	swapchain& swapchain::operator=(swapchain&& other) noexcept {
		if (m_swapchain)
			vkDestroySwapchainKHR(m_device, m_swapchain, m_allocator);

		m_swapchain = other.m_swapchain;
		m_allocator = other.m_allocator;
		m_device = other.m_device;

		other.m_swapchain = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
		other.m_allocator = nullptr;
	}

	swapchain::~swapchain() noexcept {
		if (m_swapchain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(m_device, m_swapchain, m_allocator);
	}

	const VkSwapchainKHR* swapchain::native_swapchain() const noexcept {
		return &m_swapchain;
	}

}