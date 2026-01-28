#pragma once

#include "sampler.h"

namespace rhi::primitive {

	struct sampler_vkAccess {

		static VkSampler get(const sampler& s) {
			return s.m_sampler;
		}

	};

}