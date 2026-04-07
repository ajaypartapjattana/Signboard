#pragma once

#include "commandPool.h"

namespace rhi::access {

	struct commandPool_pAccess {

		static VkCommandPool get(const rhi::pmvCommandPool& cp) noexcept {
			return cp.m_commandPool;
		}

		static VkDevice get_device(const rhi::pmvCommandPool& cp) noexcept {
			return cp._dvc;
		}

	};

}