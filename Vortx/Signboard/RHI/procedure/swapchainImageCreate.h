#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

#include "Signboard/Core/core.h"

namespace rhi {

	class device;

	class swapchain;
	class pmvImage;

	class pcdSwapchainImageAllocate {
	public:
		pcdSwapchainImageAllocate(const device& device, VkImageViewCreateInfo* pCreateInfo = nullptr) noexcept;

		void target_swapchain(const swapchain& swapchain) noexcept;
		
		uint32_t get_imageCount() const noexcept;
		VkResult publish(pmvImage& image, uint32_t index) noexcept;

		void preset(VkImageViewCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkImageViewCreateInfo* allot_basic(VkImageViewCreateInfo* pCreateInfo) noexcept;

	private:
		const VkDevice r_device;
		
		const swapchain* pSwapchain;
		std::vector<VkImage> images;

		std::unique_ptr<VkImageViewCreateInfo> m_ownedInfo;
		VkImageViewCreateInfo* pInfo;

	};

}