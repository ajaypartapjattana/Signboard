#pragma once

#include "buffer.h"

namespace rhi::primitive {

	struct buffer_vkAccess {

		static VkBuffer get(const buffer& b) noexcept {
			return b.m_buffer;
		}

		static VmaAllocation get_allocation(const buffer& b) noexcept {
			return b.m_allocation;
		}

	};

}