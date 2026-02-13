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

	renderPass_builder& renderPass_builder::add_colorAttachment(const rhi::primitive::image& image, const attachment_desc& desc) {
		VkAttachmentDescription attachment{};
		attachment.format = rhi::primitive::image_vkAccess::get_format(image);
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = desc.initialLayout;
		attachment.finalLayout = desc.finalLayout;

		m_attachments.push_back(attachment);

		VkAttachmentReference reference{};
		reference.attachment = static_cast<uint32_t>(m_attachments.size() - 1);
		reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		m_attachmentRef.push_back(reference);

		return *this;
	}

	VkResult renderPass_builder::build_graphicsPass(rhi::primitive::renderPass& tw_renderPass) {
		if (m_attachments.empty())
			return VK_INCOMPLETE;

		std::vector<VkAttachmentReference> colorAttachmentRef;

		for (const VkAttachmentReference& r : m_attachmentRef) {
			if (r.layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
				colorAttachmentRef.push_back(r);
		}

		VkSubpassDescription subpassDesc{};
		subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDesc.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRef.size());
		subpassDesc.pColorAttachments = colorAttachmentRef.data();

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(m_attachments.size());
		renderPassInfo.pAttachments = m_attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDesc;

		VkRenderPass vk_renderPass = VK_NULL_HANDLE;
		VkResult result = vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &vk_renderPass);

		tw_renderPass.m_renderPass = vk_renderPass;
		tw_renderPass.m_device = m_device;

		return result;
	}

}