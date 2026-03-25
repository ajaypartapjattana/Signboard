#pragma once

#include "pushConstantRange.h"

namespace rhi::access {

	struct pushConstantRange_pAccess {

		static VkPushConstantRange get(const rhi::pmvPushConstantRange& pcr) noexcept {
			return pcr.m_range;
		}

	};

}