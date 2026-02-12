#pragma once

#include "pushConstantRange.h"

namespace rhi::primitive {

	struct pushConstantRange_vkAccess {

		static VkPushConstantRange get(const pushConstantRange& pcr) noexcept {
			return pcr.m_range;
		}

	};

}