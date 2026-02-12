#include "swapchain.h"

namespace rhi::core {

	swapchain::swapchain(swapchain&& other) noexcept {
		m_swapchain = other.m_swapchain;
		m_format = other.m_format;
		m_extent = other.m_extent;
		m_device = other.m_device;

		other.m_swapchain = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
	}

	swapchain& swapchain::operator=(swapchain&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_swapchain)
			vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

		m_swapchain = other.m_swapchain;
		m_format = other.m_format;
		m_extent = other.m_extent;
		m_device = other.m_device;

		other.m_swapchain = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;

		return *this;
	}

	swapchain::~swapchain() noexcept {
		if (m_swapchain)
			vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
	}

	VkSwapchainKHR swapchain::native_swapchain() const noexcept {
		return m_swapchain;
	}

}