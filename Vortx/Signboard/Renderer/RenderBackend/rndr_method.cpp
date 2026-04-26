#include "rndr_method.h"

#include "Signboard/Resources/resources_pAccess.h"
#include "Signboard/Renderer/Context/render_context_Access.h"
#include "rndr_presentation_Access.h"

#include "Signboard/Assets/io/io.h"

#include <external/spirv_cross/spirv_cross.hpp>

rndr_method::rndr_method(const RHIContext& context, const rndr_presentation& presentation, const ResourceView& view)
	:
	r_device(rndr_context_Access::get_device(context)),
	r_swapchain(rndr_presentation_Access::get_swapchain(presentation)),
	r_resources(view),

	m_passes(r_device, r_resources.images),
	m_materials(r_device, r_swapchain, m_passes, m_fields)
{
	VkResult result = create_primaryTarget(presentation.expose_swapchainImages());
}

VkResult rndr_method::create_primaryTarget(ctnr::span<const uint32_t> swapchainImageIndices) {
	uint32_t renderPass_idx = m_passes.createSwapchainRenderPass(r_swapchain);

	passes::framebufferCreateInfo passInfo{};
	passInfo.attachmentImageIndices.resize(1);

	std::vector<uint32_t> framebuffer_idxs(swapchainImageIndices.size());
	for (size_t i = 0; i < swapchainImageIndices.size(); ++i) {
		passInfo.attachmentImageIndices[0] = swapchainImageIndices[i];
		framebuffer_idxs[i] = m_passes.createFramebuffers(renderPass_idx, passInfo);
	}

	vertexFields::createInfo fieldsInfo{};
	fieldsInfo.orderedFormats = {
		VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R32G32_SFLOAT,
		VK_FORMAT_R32G32B32A32_SFLOAT
	};

	uint32_t vertexLayout_idx = m_fields.createVertexLayout(fieldsInfo);

	materials::pipelineCreateInfo matInfo{};
	matInfo.shaders.resize(2);

	io::loader::file_loader _loader;

	matInfo.shaders[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	matInfo.shaders[0].data = _loader.load_SPIRV("shaders/baseDS.vert.spv");

	matInfo.shaders[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	matInfo.shaders[1].data = _loader.load_SPIRV("shaders/base.frag.spv");
	
	uint32_t pipelineLayout_idx = m_materials.createPipelineLayout(matInfo.shaders);

	uint32_t pipeline_idx = m_materials.createPipeline(renderPass_idx, 0, pipelineLayout_idx, matInfo);

	auto _ctor = [&](renderTarget* rtg) {
		renderTarget& target = *rtg;

		target.renderPassIndex = renderPass_idx;
		target.framebufferIndices = std::move(framebuffer_idxs);
		target.pipelineIndices.push_back(pipeline_idx);
	};

	ctnr::vault_writeAccessor<renderTarget> _wrt{ m_renderTargets };
	primaryTarget_idx = _wrt.construct(_ctor);
	
	return VK_SUCCESS;
}

void rndr_method::create_renderTarget(renderTargetCreateInfo& creatInfo) {
	uint32_t renderPass_idx = m_passes.createRenderPass(creatInfo.renderPassInfo);

	uint32_t framebuffer_idx = m_passes.createFramebuffers(renderPass_idx, creatInfo.framebufferInfo);

	uint32_t vertexLayout_idx = m_fields.createVertexLayout(creatInfo.VertexLayoutInfo);
	uint32_t pipelineLayout_idx = m_materials.createPipelineLayout(creatInfo.pipelineInfo.shaders);

	uint32_t pipeline_idx = m_materials.createPipeline(renderPass_idx, 0, pipelineLayout_idx, creatInfo.pipelineInfo);

	auto builder = [&](renderTarget* rtg) {
		renderTarget& target = *rtg;

		target.renderPassIndex = renderPass_idx;
		target.framebufferIndices.push_back(framebuffer_idx);
		target.pipelineIndices.push_back(pipeline_idx);
	};

	ctnr::vault_writeAccessor<renderTarget> _wrt{ m_renderTargets };
	primaryTarget_idx = _wrt.construct(builder);
}

void rndr_method::validate_primaryTarget(ctnr::span<const uint32_t> swapchainImageIndices) {
	ctnr::vault_writeAccessor<renderTarget> _wrt{ m_renderTargets };
	renderTarget& primary = *_wrt.get(primaryTarget_idx);

	passes::framebufferCreateInfo passInfo{};
	passInfo.attachmentImageIndices.resize(1);

	size_t _iSz = swapchainImageIndices.size();
	primary.framebufferIndices.resize(_iSz);
	for (size_t i = 0; i < _iSz; ++i) {
		passInfo.attachmentImageIndices[0] = swapchainImageIndices[i];
		primary.framebufferIndices[i] = m_passes.createFramebuffers(primary.renderPassIndex, passInfo);
	}

}

const ctnr::vltView<renderTarget> rndr_method::read_renderTargets() const noexcept {
	return ctnr::vltView<renderTarget>{ m_renderTargets };
}