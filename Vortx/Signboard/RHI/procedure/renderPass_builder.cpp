#include "renderPass_builder.h"

#include "Signboard/RHI/primitive/renderPass.h"
#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/primitive/image_vk.h"

#include <stdexcept>

namespace rhi::procedure {

	renderPass_builder::renderPass_builder(const rhi::core::device& device)
		: m_device(rhi::core::device_vkAccess::get(device))
	{

	}

	renderPass_builder& renderPass_builder::add_attachment(const rhi::primitive::image& image) {
		VkAttachmentDescription attachment{};
		attachment.format = rhi::primitive::image_vkAccess::get_format(image);
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		m_attachments.push_back(attachment);

		return *this;
	}

	VkResult renderPass_builder::build(rhi::primitive::renderPass& tw_renderPass) {
		if (m_attachments.empty())
			return VK_INCOMPLETE;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(m_attachments.size());
		renderPassInfo.pAttachments = m_attachments.data();

		VkRenderPass vk_renderPass = VK_NULL_HANDLE;
		VkResult result = vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &vk_renderPass);

		tw_renderPass.m_renderPass = vk_renderPass;
		tw_renderPass.m_device = m_device;

		return result;
	}

}