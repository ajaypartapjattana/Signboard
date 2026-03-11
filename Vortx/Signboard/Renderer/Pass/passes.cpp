#include "passes.h"

#include "Signboard/Renderer/RenderBackend/rndr_context_Access.h"
#include "Signboard/Renderer/RenderBackend/rndr_presentation_Access.h"

passes::passes(const rndr_context& context, const rndr_presentation& presentation) 
	: r_device(rndr_context_Access::get_device(context)),
	r_swapchain(rndr_presentation_Access::get_swapchain(presentation))
{
	setup_basePass();
}

void passes::setup_basePass() {
	rhi::procedure::renderPass_builder builder{ r_device };

	rhi::procedure::renderPass_builder::attachment_desc desc{};
	desc.format = r_swapchain.native_format();
	desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	desc.usageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	builder.add_colorAttachment(nullptr, desc);
	builder.build_graphicsPass(m_primiaryPass);
}