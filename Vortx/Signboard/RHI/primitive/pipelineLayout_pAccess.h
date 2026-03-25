#pragma once

#include "pipelineLayout.h"

namespace rhi::access {

	struct pipelineLayout_pAccess {

		static VkPipelineLayout get(const rhi::pmvPipelineLayout& pl) noexcept {
			return pl.m_layout;
		}

	};

}