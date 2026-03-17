#include "passes.h"

passes::passes(const rhi::core::device& device) 
	: 
	r_device(device),

	rp_writeAccess(m_renderPasses),
	fb_writeAccess(m_framebuffers)
{

}

storage::storage_handle passes::create_pass(const createInfo* info) {
	const createInfo& a_info = *info;

	rhi::procedure::renderPass_builder::attachment_desc desc{};
	desc.format = a_info.tu_swapchain ? a_info.tu_swapchain->native_format() : a_info.format;
	desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	desc.usageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	rhi::procedure::renderPass_builder prcdr{ r_device };
	prcdr.add_colorAttachment(nullptr, desc);

	auto builder = [&](rhi::primitive::renderPass* pass) {
		prcdr.build_graphicsPass(*pass);
	};

	return rp_writeAccess.construct(builder);
}

const storage::vault_readAccessor<rhi::primitive::renderPass> passes::get_rp_readAccessor() const noexcept {
	storage::vault_readAccessor<rhi::primitive::renderPass> accessor{ m_renderPasses };
	return accessor;
}

void passes::create_framebuffers(const storage::storage_handle passHandle, const createInfo* info, std::vector<storage::storage_handle>& fb_handles) {
	const createInfo& a_info = *info;

	rhi::procedure::framebuffer_creator prcdr{ r_device };
	prcdr.bind_renderpass(*rp_writeAccess.get(passHandle));

	if (a_info.tu_swapchain) {
		prcdr.create_swapchainTarget_framebuffer(*a_info.tu_swapchain, fb_writeAccess, fb_handles);
		return;
	}

}

const storage::vault_readAccessor<rhi::primitive::framebuffer> passes::get_fb_readAccessor() const noexcept {
	storage::vault_readAccessor<rhi::primitive::framebuffer> accessor{ m_framebuffers };
	return accessor;
}