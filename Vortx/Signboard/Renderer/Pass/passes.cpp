#include "passes.h"

passes::passes(const rhi::creDevice& device) 
	: 
	r_device(device),

	rp_writeAccess(m_renderPasses),
	fb_writeAccess(m_framebuffers)
{

}

uint32_t passes::createRenderPass(const createInfo* info) {
	const createInfo& a_info = *info;

	rhi::pcdRenderPassBuilder::attachment_desc desc{};
	desc.format = a_info.tu_swapchain ? a_info.tu_swapchain->native_format() : a_info.format;
	desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	desc.usageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	rhi::pcdRenderPassBuilder prcdr{ r_device };
	prcdr.add_colorAttachment(nullptr, desc);

	auto builder = [&](rhi::pmvRenderPass* pass) {
		prcdr.build_graphicsPass(*pass);
	};

	return rp_writeAccess.construct(builder);
}

const ctnr::vltView<rhi::pmvRenderPass> passes::read_renderPasses() const noexcept {
	return ctnr::vltView<rhi::pmvRenderPass>{ m_renderPasses };
}

void passes::createFramebuffers(const uint32_t renderPassIndex, std::vector<uint32_t>& fb_handles, const createInfo* info) {
	const createInfo& a_info = *info;

	rhi::pcdFramebufferCreator prcdr{ r_device };
	prcdr.bind_renderpass(*rp_writeAccess.get(renderPassIndex));

	if (a_info.tu_swapchain) {
		prcdr.create_swapchainTarget_framebuffer(*a_info.tu_swapchain, fb_writeAccess, fb_handles);
		return;
	}

}

const ctnr::vltView<rhi::pmvFramebuffer> passes::read_framebuffers() const noexcept {
	return ctnr::vltView<rhi::pmvFramebuffer>{ m_framebuffers };
}