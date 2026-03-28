#include "rndr_method.h"

#include "rndr_context_Access.h"
#include "rndr_presentation_Access.h"

rndr_method::rndr_method(const rndr_context& context, const rndr_presentation& presentation) 
	:
	r_device(rndr_context_Access::get_device(context)),
	r_swapchain(rndr_presentation_Access::get_swapchain(presentation)),

	m_passes(r_device),
	m_materials(r_device, r_swapchain, m_passes.get_renderPassReadAccess(), m_fields.get_vertexLayoutReadAccess()),

	m_writeAccess(targets)
{
	create_primaryTarget();
}

void rndr_method::create_primaryTarget() {
	passes::createInfo passInfo{};
	passInfo.tu_swapchain = &r_swapchain;
	
	vertexFields::createInfo fieldInfo{};
	fieldInfo.orderedFormats = { VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT };

	materials::createInfo matInfo{};
	matInfo.vertShader_path = "shaders/base.vert.spv";
	matInfo.fragShader_path = "shaders/base.frag.spv";

	createInfo info = { passInfo, fieldInfo, matInfo };
	
	create_renderTarget(info);
}

void rndr_method::create_renderTarget(const createInfo& info) {
	auto builder = [&](render_target* tgt) {
		render_target& tw_target = *tgt;

		tw_target.pass = m_passes.createRenderPass(&info.passInfo);
		m_passes.createFramebuffers(tw_target.pass, &info.passInfo, tw_target.framebuffers);

		storage::storage_handle vertexLayoutHandle = m_fields.createVertexLayout(info.fieldInfo);

		storage::storage_handle pipeHandle = m_materials.createPipeline(tw_target.pass, vertexLayoutHandle, 0, info.materialInfo);
		tw_target.pipelines.push_back(pipeHandle);
	};

	m_primaryTarget_handle = m_writeAccess.construct(builder);

}

void rndr_method::validate_primaryTarget() {
	render_target& primary = *m_writeAccess.get(m_primaryTarget_handle);

	passes::createInfo info{};
	info.tu_swapchain = &r_swapchain;

	m_passes.createFramebuffers(primary.pass, &info, primary.framebuffers);
}

const storage::vault_readAccessor<render_target> rndr_method::get_readAccessor() const noexcept {
	storage::vault_readAccessor<render_target> accessor{ targets };
	return accessor;
}