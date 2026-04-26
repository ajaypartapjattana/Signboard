#include "swapchainImageCreate.h"

#include "Signboard/RHI/primitive/image.h"
#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/swapchain_pAccess.h"

namespace rhi {

	pcdSwapchainImageAllocate::pcdSwapchainImageAllocate(const creDevice& device, const pmvSwapchain& swapchain, VkImageViewCreateInfo* pCreateInfo) noexcept
		:
		r_device(access::device_pAccess::extract(device)),
		r_swapchain(access::swapchain_pAccess::extract(swapchain)),
		format(access::swapchain_pAccess::get_format(swapchain)),
		extent(access::swapchain_pAccess::get_extent(swapchain)),

		_info(fetch_basic(pCreateInfo))
	{
		uint32_t _expImageCt = 0;
		vkGetSwapchainImagesKHR(r_device, r_swapchain, &_expImageCt, nullptr);
		images.resize(_expImageCt);
		vkGetSwapchainImagesKHR(r_device, r_swapchain, &_expImageCt, images.data());

	}

	VkImageViewCreateInfo pcdSwapchainImageAllocate::fetch_basic(VkImageViewCreateInfo* pCreateInfo) const noexcept {
		if (pCreateInfo)
			return *pCreateInfo;

		VkImageViewCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = format;

		info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = 1;

		return info;
	}

	uint32_t pcdSwapchainImageAllocate::get_imageCount() const noexcept {
		return static_cast<uint32_t>(images.size());
	}
	
	VkResult pcdSwapchainImageAllocate::publish(pmvImage& target, uint32_t index) noexcept {
		if (index >= images.size())
			return VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR;

		_info.image = images[index];

		VkImageView _view;
		VkResult result = vkCreateImageView(r_device, &_info, nullptr, &_view);
		if (result != VK_SUCCESS) {
			return result;
		}

		target.reset();

		target.m_image = _info.image;
		target.m_view = _view;

		target.format = format;
		target.extent = { extent.width, extent.height, 1 };

		target.mip_levels = 1;
		target.array_layers = 1;

		target.r_device = r_device;

		return VK_SUCCESS;
	}

}