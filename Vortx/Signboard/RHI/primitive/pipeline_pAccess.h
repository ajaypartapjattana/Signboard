#pragma once

#include "pipeline.h"

namespace rhi::access {

	struct pipeline_pAccess {

		static VkPipeline get(const rhi::pmvPipeline& p) noexcept {
			return p.m_pipeline;
		}

		static VkPipelineBindPoint get_type(const rhi::pmvPipeline& p) noexcept {
			return p.m_type;
		}

	};

}