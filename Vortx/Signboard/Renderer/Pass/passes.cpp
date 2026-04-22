#include "passes.h"

passes::passes(const rhi::creDevice& device) 
	: 
	r_device(device)
{

}

void passes::createSwapchainFramebuffer(const rhi::pmvSwapchain& swapchain) {

}

uint32_t passes::createRenderPass(const createInfo* info) {
	rhi::pcdRenderPassCreate prcdr{ r_device };

	std::vector<VkAttachmentDescription> attachments(1);

	attachments[0].flags = 0;
	attachments[0].format = info->tu_swapchain->native_format();
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	prcdr.target_attachments(attachments);

	uint32_t _subpass = prcdr.issue_subpass(VK_PIPELINE_BIND_POINT_GRAPHICS);

	std::vector<VkAttachmentReference> colorReferences(1);

	colorReferences[0].attachment = 0;
	colorReferences[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	prcdr.refer_colorAttachments(_subpass, colorReferences);

	std::vector<VkSubpassDependency> dependancies(1);

	dependancies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependancies[0].dstSubpass = 0;
	dependancies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependancies[0].srcAccessMask = 0;
	dependancies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependancies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	prcdr.target_dependancies(dependancies);

	auto builder = [&](rhi::pmvRenderPass* renderPass) {
		prcdr.publish(*renderPass);
	};

	ctnr::vault_writeAccessor<rhi::pmvRenderPass> _wrt{ m_renderPasses };
	return _wrt.construct(builder);
}

const ctnr::vltView<rhi::pmvRenderPass> passes::read_renderPasses() const noexcept {
	return ctnr::vltView<rhi::pmvRenderPass>{ m_renderPasses };
}

uint32_t passes::createFramebuffers(const uint32_t renderPassIndex, const createInfo* info) {
	const createInfo& a_info = *info;

	rhi::pcdFramebufferCreate prcdr{ r_device, *m_renderPasses[renderPassIndex] };

	prcdr.push_attachments();

	if (a_info.tu_swapchain) {
		prcdr.create_swapchainTarget_framebuffer(*a_info.tu_swapchain, fb_writeAccess, fb_handles);
		return;
	}

}

const ctnr::vltView<rhi::pmvFramebuffer> passes::read_framebuffers() const noexcept {
	return ctnr::vltView<rhi::pmvFramebuffer>{ m_framebuffers };
}