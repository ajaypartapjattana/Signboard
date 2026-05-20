#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdFramebufferCreate::pcdFramebufferCreate(const creDevice& device, VkFramebufferCreateInfo* pCreateInfo) noexcept
		: 
		r_device(_pAccess::extract(device)),
		pInfo(fetch_basic(pCreateInfo))
	{

	}

	VkFramebufferCreateInfo* pcdFramebufferCreate::fetch_basic(VkFramebufferCreateInfo* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<VkFramebufferCreateInfo>();

		VkFramebufferCreateInfo* _info = m_ownedInfo.get();
		_info->sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

		return _info;
	}

	VkResult pcdFramebufferCreate::target_renderPass(const pmvRenderPass& renderPass) noexcept {
		uint32_t count = _pAccess::attachmentCount(renderPass);

#ifdef _VALIDATE
		if (!m_attachmentImageViews.empty() && m_attachmentImageViews.size() != count)
			return VK_ERROR_INITIALIZATION_FAILED;
#endif

		if (count == 0)
			return VK_INCOMPLETE;

		pInfo->renderPass = _pAccess::extract(renderPass);

		renderPassAttachmentSlots = count;

		return VK_SUCCESS;
	}

	VkResult pcdFramebufferCreate::target_attachments(sgb::span<const pmvImage*> images) noexcept {
		if (images.empty())
			return VK_INCOMPLETE;

		VkExtent3D extent_t = _pAccess::extent(*images[0]);

		size_t _ISz = images.size();

#ifdef _VALIDATE
		if (renderPassAttachmentSlots && _ISz != renderPassAttachmentSlots)
			return VK_ERROR_INITIALIZATION_FAILED;

		for (size_t i = 0; i < _ISz; ++i) {
			VkExtent3D attachmentExtent = _pAccess::extent(*images[i]);
			if (attachmentExtent.width != extent_t.width || attachmentExtent.height != extent_t.height)
				return VK_ERROR_INITIALIZATION_FAILED;
		}
#endif

		m_attachmentImageViews.resize(_ISz);
		for (size_t i = 0; i < _ISz; ++i) {
			m_attachmentImageViews[i] = _pAccess::view(*images[i]);
		}

		pInfo->width = extent_t.width;
		pInfo->height = extent_t.height;
		pInfo->attachmentCount = static_cast<uint32_t>(m_attachmentImageViews.size());
		pInfo->pAttachments = m_attachmentImageViews.data();

		return VK_SUCCESS;
	}

	VkResult pcdFramebufferCreate::publish(pmvFramebuffer& target) {
		if (m_attachmentImageViews.empty())
			return VK_INCOMPLETE;

		VkFramebuffer _framebuffer;
		VkResult result = vkCreateFramebuffer(r_device, pInfo, nullptr, &_framebuffer);
		if (result != VK_SUCCESS)
			return result;

		target.reset();

		target.m_framebuffer = _framebuffer;
		target.extent = { pInfo->width, pInfo->height };
		target.r_device = r_device;

		return result;
	}

	void pcdFramebufferCreate::preset(VkFramebufferCreateInfo* pCreateInfo) noexcept {
		m_ownedInfo.reset();
		pInfo = fetch_basic(pCreateInfo);
	}

	void pcdFramebufferCreate::reset() noexcept {
		m_ownedInfo.reset();
		pInfo = fetch_basic(nullptr);
	}

}