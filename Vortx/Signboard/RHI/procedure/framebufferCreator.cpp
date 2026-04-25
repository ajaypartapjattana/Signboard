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

	VkResult pcdFramebufferCreate::push_attachments(ctnr::span<const pmvImage> images) {
		if (images.size() != renderPass_attachmentFormats.size())
			return VK_INCOMPLETE;

		uint32_t _ISz = static_cast<uint32_t>(images.size());

		VkExtent3D extent_t = access::image_pAccess::get_extent(images[0]);
		for (uint32_t i = 0; i < _ISz; ++i) {
			VkFormat attachmentFormat = access::image_pAccess::get_format(images[i]);
			if (attachmentFormat != renderPass_attachmentFormats[i]) {
				return VK_INCOMPLETE;
			}
			VkExtent3D attachmentExtent = access::image_pAccess::get_extent(images[i]);
			if (attachmentExtent.width != extent_t.width || attachmentExtent.height != extent_t.height)
				return VK_INCOMPLETE;
		}

		m_attachmentImageViews.clear();
		m_attachmentImageViews.reserve(_ISz);
		for (const pmvImage& image : images) {
			m_attachmentImageViews.push_back(access::image_pAccess::get_view(image));
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