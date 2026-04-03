#include "swapchain.h"

namespace rhi {

	pmvSwapchain::pmvSwapchain() noexcept
		: 
		m_swapchain(VK_NULL_HANDLE),

		m_format(VK_FORMAT_UNDEFINED),
		m_extent(),

		m_device(VK_NULL_HANDLE)
	{

	}

	pmvSwapchain::pmvSwapchain(pmvSwapchain&& other) noexcept
		:
		m_swapchain(other.m_swapchain),
		m_images(other.m_images),
		m_views(other.m_views),
		
		m_extent(other.m_extent),
		m_format(other.m_format),

		m_device(other.m_device)
	{
		other.m_swapchain = VK_NULL_HANDLE;
		other.m_views.clear();
	}

	pmvSwapchain& pmvSwapchain::operator=(pmvSwapchain&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_swapchain)
			vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

		m_swapchain = other.m_swapchain;
		m_images = other.m_images;
		m_views = other.m_views;

		m_extent = other.m_extent;
		m_format = other.m_format;
		
		m_device = other.m_device;

		other.m_swapchain = VK_NULL_HANDLE;
		other.m_views.clear();

		return *this;
	}

	pmvSwapchain::~pmvSwapchain() noexcept {
		for (VkImageView scnView : m_views)
			vkDestroyImageView(m_device, scnView, nullptr);

		if (m_swapchain)
			vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
	}

	VkFormat pmvSwapchain::native_format() const noexcept {
		return m_format;
	}

	VkExtent2D pmvSwapchain::native_extent() const noexcept {
		return m_extent;
	}

	uint32_t pmvSwapchain::native_imageCount() const noexcept {
		return static_cast<uint32_t>(m_images.size());
	}

}