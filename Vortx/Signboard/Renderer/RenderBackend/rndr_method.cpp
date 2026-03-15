#include "rndr_method.h"

#include "rndr_context_Access.h"
#include "rndr_presentation_Access.h"

rndr_method::rndr_method(const rndr_context& context, const rndr_presentation& presentation) 
	:
	r_device(rndr_context_Access::get_device(context)),
	r_swapchain(rndr_presentation_Access::get_swapchain(presentation)),

	m_passes(r_device, r_swapchain),
	m_materials(r_device, r_swapchain, m_passes)
{

}

void rndr_method::create_renderTarget() {
	passes::createInfo passInfo{};
	passInfo.swapchain_pass = true;

	storage::storage_handle passHandle = m_passes.create_pass(&passInfo);

	materials::createInfo matInfo{};
	matInfo.vertShader_path = "shaders/base.vert.spv";
	matInfo.fragShader_path = "shaders/base.frag.spv";

	storage::storage_handle pipeHandle = m_materials.create_pipeline(passHandle, 0, &matInfo);

}