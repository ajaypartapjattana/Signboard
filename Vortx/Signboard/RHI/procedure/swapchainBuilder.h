#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>

namespace rhi {

	class creDevice;
	class creSurface;

	class pmvSwapchain;

	class pcdSwapchainBuilder {
	public:
		pcdSwapchainBuilder(const rhi::creDevice& device, const rhi::creSurface& surface);

		pcdSwapchainBuilder& prefer_format(VkFormat format);
		pcdSwapchainBuilder& prefer_presentMode(VkPresentModeKHR presentMode);

		pcdSwapchainBuilder& set_extent(uint32_t w, uint32_t h);
		pcdSwapchainBuilder& set_imageCount(uint32_t count);

		pcdSwapchainBuilder& allow_tearing(bool);

		pcdSwapchainBuilder& recycle_swapchain(const rhi::pmvSwapchain&);

		VkResult build(rhi::pmvSwapchain& target_swapchain);

	private:
		VkDevice m_device;
		VkPhysicalDevice m_phys;
		VkSurfaceKHR m_surface;
		VkSwapchainKHR recycled_swapchain = VK_NULL_HANDLE;

		VkSurfaceCapabilitiesKHR surface_caps;
		std::vector<VkSurfaceFormatKHR> available_surfaceFormat;
		std::vector<VkPresentModeKHR> available_presentMode;

		VkExtent2D final_extent;

		uint32_t final_imageCount = 2;

		VkSurfaceFormatKHR final_format{};
		bool format_chosen = false;

		VkPresentModeKHR final_presentMode{};
		bool presentMode_chosen = false;

	};

}