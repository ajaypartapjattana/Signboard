	#pragma once


namespace rhi::core {
	class swapchain;
	class device;
	class surface;
}

#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>

namespace rhi::procedure {

	class swapchain_builder {
	public:
		swapchain_builder(const rhi::core::device& device, const rhi::core::surface& surface);

		swapchain_builder& prefer_format(VkFormat format);
		swapchain_builder& prefer_presentMode(VkPresentModeKHR presentMode);

		swapchain_builder& set_extent(uint32_t w, uint32_t h);
		swapchain_builder& set_imageCount(uint32_t count);

		swapchain_builder& allow_tearing(bool);

		swapchain_builder& recycle_swapchain(const rhi::core::swapchain&);

		VkResult build(rhi::core::swapchain& target_swapchain);		

	private:
		VkDevice m_device;
		VkPhysicalDevice m_phys;
		VkSurfaceKHR m_surface;
		VkSwapchainKHR recycled_swapchain = VK_NULL_HANDLE;

		VkSurfaceCapabilitiesKHR surface_caps;
		std::vector<VkSurfaceFormatKHR> available_surfaceFormat;
		std::vector<VkPresentModeKHR> available_presentMode;

		VkExtent2D final_extent{};
		bool extent_chosen = false;

		uint32_t final_imageCount = 2;
		bool imageCount_chosen = false;

		VkSurfaceFormatKHR final_format{};
		bool format_chosen = false;

		VkPresentModeKHR final_presentMode{};
		bool presentMode_chosen = false;

	};

}