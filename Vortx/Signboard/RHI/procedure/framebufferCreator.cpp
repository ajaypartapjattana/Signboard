#include "framebufferCreator.h"

#include "Signboard/RHI/primitive/framebuffer.h"

#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/swapchain_pAccess.h"
#include "Signboard/RHI/primitive/renderPass_pAccess.h"
#include "Signboard/RHI/primitive/image_pAccess.h"

namespace rhi {

	pcdFramebufferCreate::pcdFramebufferCreate(const creDevice& device, const pmvRenderPass& renderPass, VkFramebufferCreateInfo* pCreateInfo) noexcept
		: 
		r_device(access::device_pAccess::extract(device)),
		r_renderPass(access::renderPass_pAccess::extract(renderPass)),
		renderPass_attachmentFormats(access::renderPass_pAccess::get_orderedAttachmentFormats(renderPass)),
		_info(fetch_basic(pCreateInfo))
	{

	}

	VkFramebufferCreateInfo pcdFramebufferCreate::fetch_basic(VkFramebufferCreateInfo* pCreateInfo) const noexcept {
		if (pCreateInfo)
			return *pCreateInfo;

		VkFramebufferCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = r_renderPass;
		info.layers = 1;

		return info;
	}

	VkResult pcdFramebufferCreate::push_attachments(const ctnr::vltView<rhi::pmvImage>& images, ctnr::span<const uint32_t> imageHandles) {
		if (imageHandles.empty())
			return VK_INCOMPLETE;

#ifdef _VALIDATE
		if (imageHandles.size() != renderPass_attachmentFormats.size())
			return VK_INCOMPLETE;
#endif

		VkExtent3D extent_t = access::image_pAccess::get_extent(*images.get(imageHandles[0]));

		size_t _ISz = imageHandles.size();

#ifdef _VALIDATE
		for (size_t i = 0; i < _ISz; ++i) {
			const pmvImage& _img = *images.get(imageHandles[i]);

			VkFormat attachmentFormat = access::image_pAccess::get_format(_img);
			if (attachmentFormat != renderPass_attachmentFormats[i]) {
				return VK_INCOMPLETE;
			}
			VkExtent3D attachmentExtent = access::image_pAccess::get_extent(_img);
			if (attachmentExtent.width != extent_t.width || attachmentExtent.height != extent_t.height)
				return VK_INCOMPLETE;
		}
#endif

		m_attachmentImageViews.clear();
		m_attachmentImageViews.reserve(_ISz);
		for (size_t i = 0; i < _ISz; ++i) {
			m_attachmentImageViews.push_back(access::image_pAccess::get_view(*images.get(imageHandles[i])));
		}

		_info.width = extent_t.width;
		_info.height = extent_t.height;
		_info.attachmentCount = static_cast<uint32_t>(m_attachmentImageViews.size());
		_info.pAttachments = m_attachmentImageViews.data();

		return VK_SUCCESS;
	}

	VkResult pcdFramebufferCreate::publish(pmvFramebuffer& target) {
		if (m_attachmentImageViews.empty())
			return VK_INCOMPLETE;

		VkResult result = vkCreateFramebuffer(r_device, &_info, nullptr, &target.m_framebuffer);
		if (result != VK_SUCCESS)
			return result;

		target.extent = { _info.width, _info.height };
		target.r_device = r_device;

		return result;
	}

	void pcdFramebufferCreate::preset(VkFramebufferCreateInfo* pCreateInfo) noexcept {
		_info = fetch_basic(pCreateInfo);
	}

	void pcdFramebufferCreate::reset() noexcept {
		m_attachmentImageViews = {};
		_info = fetch_basic(nullptr);
	}

}