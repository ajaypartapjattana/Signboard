#pragma once

#include "semaphore.h"

namespace rhi::access {

	struct semaphore_pAccess {

		static VkSemaphore get(const rhi::pmvSemaphore& s) noexcept {
			return s.m_semaphore;
		}

	};

}