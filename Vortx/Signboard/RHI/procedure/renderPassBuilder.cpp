#include "renderPassBuilder.h"

#include "Signboard/RHI/primitive/renderPass.h"
#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/image_pAccess.h"

#include <stdexcept>

namespace rhi {

	pcdRenderPassBuilder::pcdRenderPassBuilder(const rhi::creDevice& device)
		: 
		_dvc(rhi::access::device_pAccess::get(device))
	{

	}

	pcdRenderPassBuilder& pcdRenderPassBuilder::add_colorAttachment(const rhi::pmvImage* image, const attachment_desc& desc) {
		VkAttachmentDescription attachment{};
		attachment.format = image ? rhi::access::image_pAccess::get_format(*image) : desc.format;
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
		reference.layout = desc.usageLayout;

		m_attachmentRef.push_back(reference);

		return *this;
	}

	VkResult pcdRenderPassBuilder::build_graphicsPass(rhi::pmvRenderPass& tw_renderPass) {
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

		VkSubpassDependency dependancy{};
		dependancy.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependancy.dstSubpass = 0;
		dependancy.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependancy.srcAccessMask = 0;
		dependancy.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependancy.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(m_attachments.size());
		renderPassInfo.pAttachments = m_attachments.empty() ? nullptr : m_attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDesc;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependancy;

		VkRenderPass vk_renderPass = VK_NULL_HANDLE;
		VkResult result = vkCreateRenderPass(_dvc, &renderPassInfo, nullptr, &vk_renderPass);

		tw_renderPass.m_renderPass = vk_renderPass;
		tw_renderPass._dvc = _dvc;

		return result;
	}

}