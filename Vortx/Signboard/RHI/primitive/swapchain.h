#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::access {
	struct swapchain_pAccess;
}

namespace rhi {

	class pcdSwapchainCreate;

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

	private:
		friend class pcdSwapchainCreate;
		friend struct rhi::access::swapchain_pAccess;

		VkSwapchainKHR m_swapchain;
		
		VkExtent2D extent;
		VkFormat format;

		VkDevice r_device;

	};

}