#include "passes.h"

#include "Signboard/Renderer/RenderBackend/rndr_context_Access.h"

passes::passes(const rndr_context& context) 
	: r_device(rndr_context_Access::get_device(context))
{

}

void passes::setup_basePass() {
	rhi::primitive::renderPass l_pass;

	rhi::procedure::renderPass_builder builder{ r_device };

	rhi::procedure::renderPass_builder::attachment_desc desc{};
	desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	desc.usageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	builder.add_colorAttachment(, desc);
	builder.build_graphicsPass(l_pass);
}