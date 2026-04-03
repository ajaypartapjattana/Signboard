#include "rndr_method.h"

#include "Signboard/Renderer/Context/render_context_Access.h"
#include "rndr_presentation_Access.h"

rndr_method::rndr_method(const RHIContext& context, const rndr_presentation& presentation) 
	:
	r_device(rndr_context_Access::get_device(context)),
	r_swapchain(rndr_presentation_Access::get_swapchain(presentation)),

	m_passes(r_device),
	m_materials(r_device, r_swapchain, m_passes.read_renderPasses(), m_fields.read_vertexLayouts()),

	m_writeAccess(targets)
{
	create_primaryTarget();
}

void rndr_method::create_primaryTarget() {
	passes::createInfo passInfo{};
	passInfo.tu_swapchain = &r_swapchain;
	
	vertexFields::createInfo fieldInfo{};
	fieldInfo.orderedFormats = {
		VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R32G32_SFLOAT,
		VK_FORMAT_R32G32B32A32_SFLOAT
	};

	materials::createInfo matInfo{};
	matInfo.vertShader_path = "shaders/base.vert.spv";
	matInfo.fragShader_path = "shaders/base.frag.spv";

	createInfo info = { passInfo, fieldInfo, matInfo };
	
	create_renderTarget(info);
}

void rndr_method::create_renderTarget(const createInfo& info) {
	auto builder = [&](renderTarget* tgt) {
		renderTarget& tw_target = *tgt;

		tw_target.renderPassIndex = m_passes.createRenderPass(&info.passInfo);
		m_passes.createFramebuffers(tw_target.renderPassIndex, tw_target.framebufferIndices, &info.passInfo);

		uint32_t vertexLayoutIndex = m_fields.createVertexLayout(info.fieldInfo);

		uint32_t pipelineIndex = m_materials.createPipeline(tw_target.renderPassIndex, vertexLayoutIndex, 0, info.materialInfo);
		tw_target.pipelineIndices.push_back(pipelineIndex);
	};

	m_primaryTarget_handle = m_writeAccess.construct(builder);

}

void rndr_method::validate_primaryTarget() {
	renderTarget& primary = *m_writeAccess.get(m_primaryTarget_handle);

	passes::createInfo info{};
	info.tu_swapchain = &r_swapchain;

	m_passes.createFramebuffers(primary.renderPassIndex, primary.framebufferIndices, &info);
}

const ctnr::vltView<renderTarget> rndr_method::read_renderTargets() const noexcept {
	return ctnr::vltView<renderTarget>{ targets };
}