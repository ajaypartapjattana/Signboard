#include "imageViewCreate.h"

#include "Signboard/RHI/primitive/image.h"
#include "Signboard/RHI/core/device_pAccess.h"

namespace rhi {

	pcdImageViewCreate::pcdImageViewCreate(const creDevice& device, const VkImageViewCreateInfo* pCreateInfo) noexcept
		:
		r_device(access::device_pAccess::extract(device)),
		_info(fetch_basic(pCreateInfo))
	{

	}

	VkImageViewCreateInfo pcdImageViewCreate::fetch_basic(const VkImageViewCreateInfo* pCreateinfo) const noexcept {
		if (pCreateinfo)
			return *pCreateinfo;

		VkImageViewCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;

		info.subresourceRange.levelCount = 1;
		info.subresourceRange.layerCount = 1;

		return info;
	}

	void pcdImageViewCreate::aspect(VkImageAspectFlags aspect) noexcept {
		_info.subresourceRange.aspectMask = aspect;
	}

	void pcdImageViewCreate::mipRange(uint32_t baseLevel, uint32_t levelCount) noexcept {
		_info.subresourceRange.baseMipLevel = baseLevel;
		_info.subresourceRange.levelCount = levelCount;
	}

	void pcdImageViewCreate::imageArray(uint32_t baseLayer, uint32_t layerCount) noexcept {
		_info.subresourceRange.baseArrayLayer = baseLayer;
		_info.subresourceRange.layerCount = layerCount;
	}

	void pcdImageViewCreate::envelop_image(pmvImage& image) noexcept {
		_info.format = image.format;
		_info.subresourceRange.levelCount = image.mip_levels;
		_info.subresourceRange.layerCount = image.array_layers;
	}

	VkResult pcdImageViewCreate::publish(pmvImage& target) {
		_info.image = target.m_image;

		VkImageView _view;
		VkResult result = vkCreateImageView(r_device, &_info, nullptr, &_view);
		if (result != VK_SUCCESS)
			return result;

		if (target.m_view)
			vkDestroyImageView(r_device, target.m_view, nullptr);

		target.m_view = _view;
		
		return VK_SUCCESS;
	}

}