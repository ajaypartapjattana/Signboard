#pragma once

#include "Signboard/Renderer/Internal/rndr_pInterface.h"

#include "Signboard/Renderer/Method/methods_sm.h"

struct PipelineStageShaders {
	uint32_t vertex;
	uint32_t fragment;
};

namespace rndr {

	class creContext;
	class crePresentation;
	class creResources;

	struct _pAccess;

	class creMethods {
	public:
		creMethods(const creContext& context, const crePresentation& presentation, const creResources& resources) noexcept;

		creMethods(const creMethods&) = delete;
		creMethods& operator=(const creMethods&) = delete;

		uint32_t loadShader(const char* path, VkShaderStageFlagBits stage);

		void create_primaryTarget(const PipelineStageShaders& shaderSet);
		void validate_primaryTarget();

	private:
		friend struct _pAccess;

		const rhi::creSwapchain& r_swapchain;
		const std::vector<uint32_t>& r_swapchainImageHandles;

		uint32_t primaryTarget_idx;

		sgb::vault<renderTarget> m_renderTargets;

		// passes
		sgb::vault<rhi::pmvRenderPass> m_renderPasses;
		sgb::vault<rhi::pmvFramebuffer> m_framebuffers;

		// shaders
		sgb::vault<shaderBinary> m_shaderBins;

		// materials
		sgb::vault<rhi::pmvDescriptorSetLayout> m_descriptorLayouts;
		sgb::vault<rhi::pmvPipelineLayout> m_pipelineLayouts;

		sgb::vault<rhi::pmvPipeline> m_pipelines;


		passes m_passCreate;
		framebuffers m_framebufferCreate;
		
		descriptorSetLayouts m_descriptorLayoutCreate;
		pipelineLayouts m_pipelineLayoutcreate;
		pipelines m_pipelineCreate;

	};

}
