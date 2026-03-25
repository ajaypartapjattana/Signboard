#pragma once

#include "commandBuffer.h"

namespace rhi::access {

	struct commandBuffer_pAccess {

		static VkCommandBuffer get(const pmvCommandBuffer& cb) noexcept {
			return cb.m_commandBuffer;
		}

		static VkCommandPool get_commandPool(const pmvCommandBuffer& cb) noexcept {
			return cb.m_commandPool;
		}

	};

}