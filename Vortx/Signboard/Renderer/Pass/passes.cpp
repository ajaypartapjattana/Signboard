#include "passes.h"

passes::passes(const rhi::core::device& device, const rhi::primitive::swapchain& swapchain) 
	: 
	r_device(device),
	r_swapchain(swapchain),

	m_writeAccess(m_renderPasses)
{

}

storage::storage_handle passes::create_pass(const createInfo* info) {
	rhi::procedure::renderPass_builder prcdr{ r_device };

	const createInfo& a_info = *info;

	rhi::procedure::renderPass_builder::attachment_desc desc{};
	desc.format = a_info.swapchain_pass ? r_swapchain.native_format() : a_info.format;
	desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	desc.usageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	prcdr.add_colorAttachment(nullptr, desc);

	auto builder = [&](rhi::primitive::renderPass* pass) {
		prcdr.build_graphicsPass(*pass);
	};

	return m_writeAccess.construct(builder);
}

storage::vault_readAccessor<rhi::primitive::renderPass> passes::get_readAccessor() const noexcept {
	storage::vault_readAccessor<rhi::primitive::renderPass> accessor{ m_renderPasses };
	return accessor;
}
