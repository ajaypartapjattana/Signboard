#pragma once

namespace rhi::procedure {
	class swapchain_builder;
}

#include <vulkan/vulkan.h>

namespace rhi::core {

	struct swapchain_vkAccess;

	class swapchain {
	public:
		swapchain() noexcept;

		swapchain(const swapchain&) = delete;
		swapchain& operator=(const swapchain&) = delete;

		swapchain(swapchain&&) noexcept;
		swapchain& operator=(swapchain&&) noexcept;

		~swapchain() noexcept;

		VkSwapchainKHR native_swapchain() const noexcept;

	private:
		friend class rhi::procedure::swapchain_builder;
		friend struct swapchain_vkAccess;

		VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
		
		VkFormat m_format = VK_FORMAT_UNDEFINED;
		VkExtent2D m_extent{};
		
		VkDevice m_device = VK_NULL_HANDLE;

	};

}