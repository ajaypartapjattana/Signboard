#include "Signboard/Renderer/Internal/rndr_pAccess.h"

namespace rndr {

	passes::passes(const rhi::device& device, sgb::vltWrite<rhi::pmvRenderPass> renderPassWrite) noexcept
		:
		m_renderPassCreate(device),
		_wrt(renderPassWrite)
	{

	}

	uint32_t passes::createSwapchainRenderPass(const rhi::creSwapchain& swapchain) {
		std::vector<VkAttachmentDescription> attachments(1);

		attachments[0].flags = 0;
		attachments[0].format = swapchain.native_format();
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		m_renderPassCreate.target_attachments(attachments);

		uint32_t _subpass = m_renderPassCreate.issue_subpass(VK_PIPELINE_BIND_POINT_GRAPHICS);

		std::vector<VkAttachmentReference> colorReferences(1);

		colorReferences[0].attachment = 0;
		colorReferences[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		m_renderPassCreate.refer_colorAttachments(_subpass, colorReferences);

		std::vector<VkSubpassDependency> dependancies(1);

		dependancies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependancies[0].dstSubpass = 0;
		dependancies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependancies[0].srcAccessMask = 0;
		dependancies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependancies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		m_renderPassCreate.target_dependancies(dependancies);

		auto builder = [&](rhi::pmvRenderPass* renderPass) {
			m_renderPassCreate.publish(*renderPass);
			};

		return _wrt.construct(builder);
	}

	uint32_t passes::createRenderPass(sgb::span<const VkAttachmentDescription> attachments) {
		m_renderPassCreate.target_attachments(attachments);

		uint32_t subpass_idx = m_renderPassCreate.issue_subpass(VK_PIPELINE_BIND_POINT_GRAPHICS);

		std::vector<VkAttachmentReference> colorReferences(1);

		colorReferences[0].attachment = 0;
		colorReferences[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		m_renderPassCreate.refer_colorAttachments(subpass_idx, colorReferences);

		std::vector<VkSubpassDependency> dependancies(1);

		dependancies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependancies[0].dstSubpass = 0;
		dependancies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependancies[0].srcAccessMask = 0;
		dependancies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependancies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		m_renderPassCreate.target_dependancies(dependancies);

		auto builder = [&](rhi::pmvRenderPass* renderPass) {
			m_renderPassCreate.publish(*renderPass);
		};

		return _wrt.construct(builder);
	}

	

}
