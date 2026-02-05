#pragma once

#include "commandBuffer.h"

namespace rhi::primitive {

	struct commandBuffer_vkAccess {

		static VkCommandBuffer get(const commandBuffer& cb) noexcept {
			return cb.m_commandBuffer;
		}

		static VkCommandPool get_commandPool(const commandBuffer& cb) noexcept {
			return cb.m_commandPool;
		}

	};

}