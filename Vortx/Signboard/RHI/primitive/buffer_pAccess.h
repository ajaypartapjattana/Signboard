#pragma once

#include "buffer.h"

namespace rhi::primitive {

	struct buffer_vkAccess {

		static VkBuffer get(const pmvBuffer& b) noexcept {
			return b.m_buffer;
		}

		static VmaAllocation get_allocation(const pmvBuffer& b) noexcept {
			return b.m_allocation;
		}

	};

}