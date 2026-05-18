#include "imageViewCreate.h"

#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdImageViewCreate::pcdImageViewCreate(const creDevice& device, VkImageViewCreateInfo* pCreateInfo) noexcept
		:
		r_device(_pAccess::extract(device)),
		pInfo(allot_basic(pCreateInfo))
	{

	}

	VkImageViewCreateInfo* pcdImageViewCreate::allot_basic(VkImageViewCreateInfo* pCreateinfo) noexcept {
		if (pCreateinfo)
			return pCreateinfo;

		m_ownedInfo = std::make_unique<VkImageViewCreateInfo>();

		VkImageViewCreateInfo* _info = m_ownedInfo.get();
		_info->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		_info->viewType = VK_IMAGE_VIEW_TYPE_2D;

		_info->subresourceRange.levelCount = 1;
		_info->subresourceRange.layerCount = 1;

		return _info;
	}

	void pcdImageViewCreate::aspect(VkImageAspectFlags aspect) noexcept {
		pInfo->subresourceRange.aspectMask = aspect;
	}

	void pcdImageViewCreate::mipLevel(uint32_t baseLevel, uint32_t levelCount) noexcept {
		pInfo->subresourceRange.baseMipLevel = baseLevel;
		pInfo->subresourceRange.levelCount = levelCount;
	}

	void pcdImageViewCreate::arrayLayer(uint32_t baseLayer, uint32_t layerCount) noexcept {
		pInfo->subresourceRange.baseArrayLayer = baseLayer;
		pInfo->subresourceRange.layerCount = layerCount;
	}

	void pcdImageViewCreate::components(VkComponentMapping mapping) noexcept {
		pInfo->components = mapping;
	}

	void pcdImageViewCreate::target_image(const pmvImage& image) noexcept {
		pInfo->image = image.m_image;
		pInfo->format = image.format;

		pInfo->subresourceRange.levelCount = image.mip_levels;
		pInfo->subresourceRange.layerCount = image.array_layers;
	}

	VkResult pcdImageViewCreate::publish(pmvImage& target) const noexcept {
		VkImageView _view;
		VkResult result = vkCreateImageView(r_device, pInfo, nullptr, &_view);
		if (result != VK_SUCCESS)
			return result;

		target.reset_view();

		target.m_view = _view;
		target.r_device = r_device;
		
		return VK_SUCCESS;
	}

}