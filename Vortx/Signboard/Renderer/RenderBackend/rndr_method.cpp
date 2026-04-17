#include "rndr_method.h"

#include "Signboard/Renderer/Context/render_context_Access.h"
#include "rndr_presentation_Access.h"

#include "Signboard/Assets/io/io.h"

#include <external/spirv_cross/spirv_cross.hpp>

rndr_method::rndr_method(const RHIContext& context, const rndr_presentation& presentation) 
	:
	r_device(rndr_context_Access::get_device(context)),
	r_swapchain(rndr_presentation_Access::get_swapchain(presentation)),

	m_passes(r_device),
	m_materials(r_device, r_swapchain, m_passes.read_renderPasses(), m_fields.read_vertexLayouts()),

	m_writeAccess(targets)
{
	VkResult result = create_primaryTarget();
}

VkResult rndr_method::create_primaryTarget() {
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

	materials::pipelineCreateInfo matInfo{};
	matInfo.shaders.resize(2);

	io::loader::file_loader _loader;

	matInfo.shaders[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	matInfo.shaders[0].data = _loader.load_SPIRV("shaders/base.vert.spv");

	matInfo.shaders[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	matInfo.shaders[1].data = _loader.load_SPIRV("shaders/base.frag.spv");
	
	create_renderTarget(passInfo, fieldInfo, matInfo);

	return VK_SUCCESS;
}

void rndr_method::create_renderTarget(const passes::createInfo& passInfo, const vertexFields::createInfo& fieldsInfo, const materials::pipelineCreateInfo& pipeInfo) {	
	uint32_t _rpIndex = m_passes.createRenderPass(&passInfo);
	uint32_t _vlIndex = m_fields.createVertexLayout(fieldsInfo);
	uint32_t _plIndex = m_materials.createPipelineLayout(pipeInfo.shaders);
	uint32_t _pIndex = m_materials.createPipeline(_rpIndex, 0, _plIndex, pipeInfo);

	auto builder = [&](renderTarget* tgt) {
		renderTarget& tw_target = *tgt;

		tw_target.renderPassIndex = _rpIndex;
		m_passes.createFramebuffers(_rpIndex, tw_target.framebufferIndices, &passInfo);
		tw_target.pipelineIndices.push_back(_pIndex);
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