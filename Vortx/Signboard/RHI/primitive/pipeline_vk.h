#pragma once

#include "pipeline.h"

namespace rhi::primitive {

	struct pipeline_vkAccess {

		static VkPipeline get(const pipeline& p) noexcept {
			return p.m_pipeline;
		}

		static VkPipelineBindPoint get_type(const pipeline& p) noexcept {
			return p.m_type;
		}

	};

}