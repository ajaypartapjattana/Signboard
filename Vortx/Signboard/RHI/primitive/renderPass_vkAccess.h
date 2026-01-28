#pragma once

#include "renderPass.h"

namespace rhi::primitive {

	struct renderPass_vkAccess {

		static VkRenderPass get(const renderPass& rp) {
			return rp.m_renderPass;
		}

	};

}