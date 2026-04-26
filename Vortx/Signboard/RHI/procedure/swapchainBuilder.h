#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>

namespace rhi {

	class creDevice;
	class creSurface;

	class pmvSwapchain;

	class pcdSwapchainCreate {
	public:
		pcdSwapchainCreate(const rhi::creDevice& device, const rhi::creSurface& surface, VkSwapchainCreateInfoKHR* pCreateInfo = nullptr) noexcept;

		void carry_surface() noexcept;

		VkResult set_transform(VkSurfaceTransformFlagBitsKHR transform);
		VkResult set_presentMode(VkPresentModeKHR presentMode) noexcept;

		VkResult set_imageFormat(VkFormat format, VkColorSpaceKHR colorSpace) noexcept;
		VkResult set_imageCount(uint32_t count) noexcept;
		void set_extent(uint32_t width, uint32_t height) noexcept;

		void recycle_swapchain(const rhi::pmvSwapchain& swapchain) noexcept;

		VkResult publish(rhi::pmvSwapchain& swapchain) const noexcept;

		void preset(VkSwapchainCreateInfoKHR* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkSwapchainCreateInfoKHR fetch_basic(VkSwapchainCreateInfoKHR* pCreateInfo) const noexcept;

	private:
		const VkDevice r_device;
		const VkPhysicalDevice m_phys;
		const VkSurfaceKHR r_surface;
		
		VkSwapchainCreateInfoKHR _info;

		VkSurfaceCapabilitiesKHR surface_caps;

	};

}