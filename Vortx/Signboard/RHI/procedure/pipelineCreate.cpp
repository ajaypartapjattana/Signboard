#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdGraphicPipelineCreate::pcdGraphicPipelineCreate(const rhi::device& device, graphicPipelineCreateArena* pCreateArena, VkGraphicsPipelineCreateInfo* pCreateInfo) noexcept
		: 
		r_device(_pAccess::extract(device)),
		pArena(allot_arena(pCreateArena)),
		pInfo(allot_basic(pCreateInfo, *pArena))
	{
	
	}

	graphicPipelineCreateArena* pcdGraphicPipelineCreate::allot_arena(graphicPipelineCreateArena* pArena) noexcept {
		if (pArena)
			return pArena;

		m_ownedArena = std::make_unique<graphicPipelineCreateArena>();

		graphicPipelineCreateArena* _arena = m_ownedArena.get();

		graphicPipelineCreateArena& arena = *_arena;
		arena.vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		arena.inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		arena.tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		arena.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		arena.rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		arena.multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		arena.depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		arena.colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		arena.dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

		return _arena;
	}

	VkGraphicsPipelineCreateInfo* pcdGraphicPipelineCreate::allot_basic(VkGraphicsPipelineCreateInfo* pCreateInfo, graphicPipelineCreateArena& arena) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<VkGraphicsPipelineCreateInfo>();

		VkGraphicsPipelineCreateInfo* _info = m_ownedInfo.get();

		VkGraphicsPipelineCreateInfo& info = *_info;
		info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

		info.pVertexInputState = &arena.vertexInputState;
		info.pInputAssemblyState = &arena.inputAssemblyState;
		info.pTessellationState = &arena.tessellationState;
		info.pViewportState = &arena.viewportState;
		info.pRasterizationState = &arena.rasterizationState;
		info.pMultisampleState = &arena.multisampleState;
		info.pDepthStencilState = &arena.depthStencilState;
		info.pColorBlendState = &arena.colorBlendState;
		info.pDynamicState = &arena.dynamicState;

		return _info;
	}

	void pcdGraphicPipelineCreate::bind_shaders(sgb::span<const pmvShader> shaders) noexcept {
		m_shaders.clear();

		if (shaders.empty())
			return;

		VkPipelineShaderStageCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info.pName = "main";

		size_t _sSz = shaders.size();
		m_shaders.reserve(_sSz);
		for (size_t i = 0; i < _sSz; ++i) {
			info.module = _pAccess::extract(shaders[i]);
			info.stage = _pAccess::stage(shaders[i]);

			m_shaders.emplace_back(info);
		}

		pInfo->stageCount = static_cast<uint32_t>(_sSz);
		pInfo->pStages = m_shaders.data();
	}

	void pcdGraphicPipelineCreate::vertexInput(sgb::span<const VkVertexInputBindingDescription> bindings, sgb::span<const VkVertexInputAttributeDescription> attributes) noexcept {
		VkPipelineVertexInputStateCreateInfo& info = pArena->vertexInputState;

		info.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
		info.pVertexBindingDescriptions = bindings.data();
		info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
		info.pVertexAttributeDescriptions = attributes.data();
	}

	void pcdGraphicPipelineCreate::inputAssembly(VkPrimitiveTopology topology) noexcept {
		VkPipelineInputAssemblyStateCreateInfo& info = pArena->inputAssemblyState;

		info.primitiveRestartEnable = VK_FALSE;
		info.topology = topology;
	}

	void pcdGraphicPipelineCreate::viewportState(uint32_t viewportCount, uint32_t scissorsCount) noexcept {
		VkPipelineViewportStateCreateInfo& info = pArena->viewportState;

		info.viewportCount = viewportCount;
		info.scissorCount = scissorsCount;
	}

	void pcdGraphicPipelineCreate::rasterCulling(VkBool32 discard, VkCullModeFlags cullMode, VkFrontFace frontFace) noexcept {
		VkPipelineRasterizationStateCreateInfo& info = pArena->rasterizationState;

		info.rasterizerDiscardEnable = discard;
		info.cullMode = cullMode;
		info.frontFace = frontFace;
	}

	void pcdGraphicPipelineCreate::rasterPolygons(VkPolygonMode mode, float lineWidth) noexcept {
		VkPipelineRasterizationStateCreateInfo& info = pArena->rasterizationState;

		info.polygonMode = mode;
		info.lineWidth = lineWidth;
	}

	void pcdGraphicPipelineCreate::rasterDepthClamping(VkBool32 depthClamp) noexcept {
		pArena->rasterizationState.depthClampEnable = depthClamp;
	}

	void pcdGraphicPipelineCreate::rasterDepthBias(VkBool32 depthBias, float factor, float clamp, float slope) noexcept {
		VkPipelineRasterizationStateCreateInfo& info = pArena->rasterizationState;

		info.depthBiasEnable = depthBias;
		info.depthBiasConstantFactor = factor;
		info.depthBiasClamp = clamp;
		info.depthBiasSlopeFactor = slope;
	}

	void pcdGraphicPipelineCreate::multisample(VkSampleCountFlagBits sampleCount) noexcept {
		VkPipelineMultisampleStateCreateInfo& info = pArena->multisampleState;

		info.rasterizationSamples = sampleCount;

		if (sampleCount != VK_SAMPLE_COUNT_1_BIT)
			info.sampleShadingEnable = VK_TRUE;
	}

	void pcdGraphicPipelineCreate::depthStencilState() noexcept {

	}

	void pcdGraphicPipelineCreate::push_blendAttachments(sgb::span<const VkPipelineColorBlendAttachmentState> blendAttachments) noexcept {
		VkPipelineColorBlendStateCreateInfo& info = pArena->colorBlendState;

		info.attachmentCount = static_cast<uint32_t>(blendAttachments.size());
		info.pAttachments = blendAttachments.data();
	}

	void pcdGraphicPipelineCreate::dynamicStates(sgb::span<const VkDynamicState> states) noexcept {
		VkPipelineDynamicStateCreateInfo& info = pArena->dynamicState;

		info.dynamicStateCount = static_cast<uint32_t>(states.size());
		info.pDynamicStates = states.data();
	}

	void pcdGraphicPipelineCreate::target_renderPass(const rhi::pmvRenderPass& renderPass, uint32_t subpass) noexcept {
		pInfo->renderPass = _pAccess::extract(renderPass);
		pInfo->subpass = subpass;
	}

	void pcdGraphicPipelineCreate::target_layout(const pmvPipelineLayout& pipelineLayout) noexcept {
		pInfo->layout = _pAccess::extract(pipelineLayout);
	}

	VkResult pcdGraphicPipelineCreate::publish(pmvPipeline& target) {
		if (m_shaders.empty())
			return VK_INCOMPLETE;

		VkPipeline _pipeline;
		VkResult result =  vkCreateGraphicsPipelines(r_device, nullptr, 1, pInfo, nullptr, &_pipeline);
		if (result != VK_SUCCESS)
			return result;

		if (target.r_device)
			vkDestroyPipeline(target.r_device, target.m_pipeline, nullptr);

		target.m_pipeline = _pipeline;
		target.type = VK_PIPELINE_BIND_POINT_GRAPHICS;
		target.r_device = r_device;

		return VK_SUCCESS;
	}

	void pcdGraphicPipelineCreate::preset(graphicPipelineCreateArena* pCreateArena, VkGraphicsPipelineCreateInfo* pCreateInfo) noexcept {
		m_ownedInfo.reset();
		m_ownedArena.reset();
		pArena = allot_arena(pCreateArena);
		pInfo = allot_basic(pCreateInfo, *pArena);
	}

	void pcdGraphicPipelineCreate::reset() noexcept {
		m_ownedInfo.reset();
		m_ownedArena.reset();
		pArena = allot_arena(nullptr);
		pInfo = allot_basic(nullptr, *pArena);
	}

}
