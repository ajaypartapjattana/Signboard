#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Core/core.h"

#include "detail/shaderBinary.h"

namespace rndr {

	class pipelines {
	public:
		pipelines(const rhi::creDevice& device, sgb::vltWrite<rhi::pmvPipeline> pipelineWrite);

		uint32_t createPipeline(
			const rhi::pmvRenderPass& renderPass,
			uint32_t subpass,
			const rhi::pmvPipelineLayout& pipelineLayout,
			sgb::span<const shaderBinary*> shaders
		);

	private:
		sgb::vltWrite<rhi::pmvPipeline> _wrt;

		rhi::pcdShaderModuleCreate m_shaderCreate;
		rhi::pcdGraphicPipelineCreate m_pipelineCreate;

	};

}

