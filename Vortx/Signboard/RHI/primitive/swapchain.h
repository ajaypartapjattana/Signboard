#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::access {
	struct swapchain_pAccess;
}

namespace rhi {

	class pcdSwapchainBuilder;

	class pmvSwapchain {
	public:
		pmvSwapchain() noexcept;

		pmvSwapchain(const pmvSwapchain&) = delete;
		pmvSwapchain& operator=(const pmvSwapchain&) = delete;

		pmvSwapchain(pmvSwapchain&&) noexcept;
		pmvSwapchain& operator=(pmvSwapchain&&) noexcept;

		~pmvSwapchain() noexcept;

		VkFormat native_format() const noexcept;
		VkExtent2D native_extent() const noexcept;
		uint32_t native_imageCount() const noexcept;

	private:
		friend class pcdSwapchainBuilder;
		friend struct rhi::access::swapchain_pAccess;

		VkSwapchainKHR m_swapchain;
		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_views;
		
		VkFormat m_format;
		VkExtent2D m_extent;
		
		VkDevice m_device;

	};

}