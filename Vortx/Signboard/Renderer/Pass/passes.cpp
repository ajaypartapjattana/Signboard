#include "passes.h"

#include "Signboard/Renderer/RenderBackend/rndr_context_Access.h"

passes::passes(const rndr_context& context) 
	: r_device(rndr_context_Access::get_device(context))
{
	setup_basePass();
}

const rhi::primitive::renderPass& passes::get_basePass() const noexcept {
	return m_basePass;
}

void passes::setup_basePass() {
	rhi::procedure::renderPass_builder builder{ r_device };

	rhi::procedure::renderPass_builder::attachment_desc desc{};
	desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	desc.usageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	builder.add_colorAttachment(nullptr, desc);
	builder.build_graphicsPass(m_basePass);
}