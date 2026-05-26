#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdSwapchainImageAllocate::pcdSwapchainImageAllocate(const device& device, VkImageViewCreateInfo* pCreateInfo) noexcept
		:
		r_device(_pAccess::extract(device)),
		pInfo(allot_basic(pCreateInfo))
	{
		
	}

	VkImageViewCreateInfo* pcdSwapchainImageAllocate::allot_basic(VkImageViewCreateInfo* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<VkImageViewCreateInfo>();

		VkImageViewCreateInfo* info = m_ownedInfo.get();
		info->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info->viewType = VK_IMAGE_VIEW_TYPE_2D;

		info->subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info->subresourceRange.baseMipLevel = 0;
		info->subresourceRange.levelCount = 1;
		info->subresourceRange.baseArrayLayer = 0;
		info->subresourceRange.layerCount = 1;

		return info;
	}

	void pcdSwapchainImageAllocate::target_swapchain(const creSwapchain& swapchain) noexcept {
		if (swapchain.m_swapchain == VK_NULL_HANDLE)
			return;

		pSwapchain = &swapchain;

		uint32_t _expImageCt = 0;
		vkGetSwapchainImagesKHR(r_device, swapchain.m_swapchain, &_expImageCt, nullptr);
		images.resize(_expImageCt);
		vkGetSwapchainImagesKHR(r_device, swapchain.m_swapchain, &_expImageCt, images.data());
	}

	uint32_t pcdSwapchainImageAllocate::get_imageCount() const noexcept {
		return static_cast<uint32_t>(images.size());
	}
	
	VkResult pcdSwapchainImageAllocate::publish(pmvImage& target, uint32_t index) noexcept {
		if (index >= images.size())
			return VK_ERROR_INVALID_EXTERNAL_HANDLE;

		VkImage image = images[index];

		pInfo->image = image;

		VkImageView _view;
		VkResult result = vkCreateImageView(r_device, pInfo, nullptr, &_view);
		if (result != VK_SUCCESS) {
			return result;
		}

		target.reset();

		target.m_image = image;
		target.m_view = _view;

		target.format = pSwapchain->format;
		target.extent = { pSwapchain->extent.width ,pSwapchain->extent.height , 1 };

		target.mip_levels = 1;
		target.array_layers = 1;

		target.r_device = r_device;

		return VK_SUCCESS;
	}

	void pcdSwapchainImageAllocate::preset(VkImageViewCreateInfo* pCreateInfo) noexcept {
		m_ownedInfo.reset();
		pInfo = allot_basic(pCreateInfo);
	}

	void pcdSwapchainImageAllocate::reset() noexcept {
		m_ownedInfo.reset();
		images.clear();
	}

}