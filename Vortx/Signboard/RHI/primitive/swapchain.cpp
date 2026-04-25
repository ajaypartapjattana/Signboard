#include "swapchain.h"

namespace rhi {

	pmvSwapchain::pmvSwapchain() noexcept
		: 
		m_swapchain(VK_NULL_HANDLE),
		format(VK_FORMAT_UNDEFINED),
		extent(),

		r_device(VK_NULL_HANDLE)
	{

	}

	pmvSwapchain::pmvSwapchain(pmvSwapchain&& other) noexcept
		:
		m_swapchain(other.m_swapchain),
		extent(other.extent),
		format(other.format),

		r_device(other.r_device)
	{
		other.m_swapchain = VK_NULL_HANDLE;
	}

	pmvSwapchain& pmvSwapchain::operator=(pmvSwapchain&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_swapchain)
			vkDestroySwapchainKHR(r_device, m_swapchain, nullptr);

		m_swapchain = other.m_swapchain;
		extent = other.extent;
		format = other.format;
		r_device = other.r_device;

		other.m_swapchain = VK_NULL_HANDLE;

		return *this;
	}

	pmvSwapchain::~pmvSwapchain() noexcept {
		if (m_swapchain)
			vkDestroySwapchainKHR(r_device, m_swapchain, nullptr);
	}

	VkFormat pmvSwapchain::native_format() const noexcept {
		return format;
	}

	VkExtent2D pmvSwapchain::native_extent() const noexcept {
		return extent;
	}

}