#include "passes.h"

#include "Signboard/Renderer/RenderBackend/rndr_interfaceAccess.h"

passes::passes(const render_interface& ri) 
	: m_device(rndr_interfaceAccess::say_device(ri))
{

}

void passes::setup_basePass() {
	rhi::primitive::renderPass l_pass;

	rhi::procedure::renderPass_builder builder{ m_device };

	rhi::procedure::renderPass_builder::attachment_desc desc{};
	desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	builder.add_colorAttachment(, desc);
	builder.build_graphicsPass(l_pass);
}