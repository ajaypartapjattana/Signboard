#pragma once

#include "fence.h"

namespace rhi::access {

	struct fence_pAccess {

		static VkFence get(const rhi::pmvFence& f) noexcept {
			return f.m_fence;
		}

	};

}