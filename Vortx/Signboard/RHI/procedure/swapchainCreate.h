#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace rhi {

	class creDevice;
	class creSurface;

	class creSwapchain;

	class pcdSwapchainCreate {
	public:
		pcdSwapchainCreate(const creDevice& device, const creSurface& surface, VkSwapchainCreateInfoKHR* pCreateInfo = nullptr) noexcept;

		VkResult imageFormat(VkFormat format, VkColorSpaceKHR colorSpace) noexcept;
		VkResult imageCount(uint32_t count) noexcept;
		VkResult presentMode(VkPresentModeKHR presentMode) noexcept;
		
		void carry_surface() noexcept;
		
		void imageExtent(uint32_t width, uint32_t height) noexcept;
		VkResult transform(VkSurfaceTransformFlagBitsKHR transform);

		void imageUsage(VkImageUsageFlags usage) noexcept;
		void arraylayers(uint32_t layerCount) noexcept;
		void enablePreserve(bool enable) noexcept;

		void recycle_swapchain(const creSwapchain& swapchain) noexcept;

		VkResult publish(creSwapchain& swapchain) const noexcept;

		void preset(VkSwapchainCreateInfoKHR* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkSwapchainCreateInfoKHR* allot_basic(VkSwapchainCreateInfoKHR* pCreateInfo) noexcept;

	private:
		const VkDevice r_device;
		const VkPhysicalDevice m_phys;
		const VkSurfaceKHR r_surface;
		
		VkSurfaceCapabilitiesKHR surface_caps;
		
		std::unique_ptr<VkSwapchainCreateInfoKHR> m_ownedInfo;
		VkSwapchainCreateInfoKHR* pInfo;

	};

}