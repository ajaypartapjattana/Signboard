#pragma once

#include "semaphore.h"

namespace rhi::primitive {

	struct semaphore_vkAccess {

		static VkSemaphore get(const semaphore& s) noexcept {
			return s.m_semaphore;
		}

	};

}