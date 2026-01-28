#pragma once

namespace rhi::procedure {
	class swapchain_builder;
}

#include <vulkan/vulkan.h>

namespace rhi::core {

	struct swapchain_vkAccess;

	class swapchain {
	public:
		swapchain(const swapchain&) = delete;
		swapchain& operator=(const swapchain&) = delete;

		swapchain(swapchain&&) noexcept;
		swapchain& operator=(swapchain&&) noexcept;

		~swapchain() noexcept;

		VkSwapchainKHR native_swapchain() const noexcept;

	private:
		friend class rhi::procedure::swapchain_builder;
		friend struct swapchain_vkAccess;

		swapchain() = default;

		VkSwapchainKHR m_swapchain;
		VkFormat m_format;
		
		VkDevice m_device;

	};

}