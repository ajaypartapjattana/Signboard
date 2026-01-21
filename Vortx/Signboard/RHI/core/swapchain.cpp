#include "swapchain.h"

#include <vulkan/vulkan.h>

namespace rhi::core {

	swapchain::swapchain(swapchain&& other) noexcept {
		m_swapchain = other.m_swapchain;
		m_allocator = other.m_allocator;
		m_device = other.m_device;

		other.m_swapchain = nullptr;
		other.m_allocator = nullptr;
		other.m_device = nullptr;
	}

	swapchain& swapchain::operator=(swapchain&& other) noexcept {
		if (m_swapchain) {
			vkDestroySwapchainKHR(
				reinterpret_cast<VkDevice>(m_device),
				reinterpret_cast<VkSwapchainKHR>(m_swapchain),
				reinterpret_cast<VkAllocationCallbacks*>(m_allocator)
			);
		}

		m_swapchain = other.m_swapchain;
		m_allocator = other.m_allocator;
		m_device = other.m_device;

		other.m_swapchain = nullptr;
		other.m_allocator = nullptr;
		other.m_device = nullptr;
	}

	swapchain::~swapchain() noexcept {
		if (m_swapchain) {
			vkDestroySwapchainKHR(
				reinterpret_cast<VkDevice>(m_device),
				reinterpret_cast<VkSwapchainKHR>(m_swapchain),
				reinterpret_cast<VkAllocationCallbacks*>(m_allocator)
			);
		}
	}

	const void* swapchain::native_swapchain() const noexcept {
		return m_swapchain;
	}

}