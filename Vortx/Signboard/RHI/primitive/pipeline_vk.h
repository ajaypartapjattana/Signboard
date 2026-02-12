#pragma once

#include "pipeline.h"

namespace rhi::primitive {

	struct pipeline_vkAccess {

		static VkPipeline get(const pipeline& p) noexcept {
			return p.m_pipeline;
		}

	};

}