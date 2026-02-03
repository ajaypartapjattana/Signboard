#pragma once

#include "commandPool.h"

namespace rhi::core {

	struct commandPool_vkAccess {

		static VkCommandPool get(const commandPool& cp) noexcept {
			return cp.m_commandPool;
		}

		static VkDevice get_device(const commandPool& cp) noexcept {
			return cp.m_device;
		}

	};

}