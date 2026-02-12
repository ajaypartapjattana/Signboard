#pragma once

#include "pipelineLayout.h"

namespace rhi::primitive {

	struct pipelineLayout_vkAccess {

		static VkPipelineLayout get(const pipelineLayout& pl) noexcept {
			return pl.m_layout;
		}

	};

}