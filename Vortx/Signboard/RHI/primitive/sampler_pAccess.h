#pragma once

#include "sampler.h"

namespace rhi::access {

	struct sampler_pAccess {

		static VkSampler get(const rhi::pmvSampler & s) {
			return s.m_sampler;
		}

	};

}