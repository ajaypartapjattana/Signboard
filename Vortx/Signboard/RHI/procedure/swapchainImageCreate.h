#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "Signboard/Core/Containers/storage.h"

namespace rhi {

	class creDevice;

	class pmvSwapchain;
	class pmvImage;

	class pcdSwapchainImageAllocate {
	public:
		pcdSwapchainImageAllocate(const creDevice& device, const pmvSwapchain& swapchain, VkImageViewCreateInfo* pCreateInfo = nullptr) noexcept;

		uint32_t get_imageCount() const noexcept;
		
		VkResult publish(pmvImage& image, uint32_t index) noexcept;

	private:
		VkImageViewCreateInfo fetch_basic(VkImageViewCreateInfo* pCreateInfo) const noexcept;

	private:
		const VkDevice r_device;
		const VkSwapchainKHR r_swapchain;
		
		const VkFormat format;
		const VkExtent2D extent;

		std::vector<VkImage> images;

		VkImageViewCreateInfo _info;

	};

}