#pragma once

#include "fence.h"

namespace rhi::primitive {

	struct fence_vkAccess {

		static VkFence get(const fence& f) noexcept {
			return f.m_fence;
		}

	};

}