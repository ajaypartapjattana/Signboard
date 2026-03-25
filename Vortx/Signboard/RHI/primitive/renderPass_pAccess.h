#pragma once

#include "renderPass.h"

namespace rhi::access {

	struct renderPass_pAccess {

		static VkRenderPass get(const rhi::pmvRenderPass& rp) {
			return rp.m_renderPass;
		}

	};

}