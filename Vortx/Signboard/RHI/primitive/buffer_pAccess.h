#pragma once

#include "buffer.h"

namespace rhi::access {

	struct buffer_pAccess {

		static VkBuffer get(const rhi::pmvBuffer& b) noexcept {
			return b.m_buffer;
		}

		static VmaAllocation get_allocation(const rhi::pmvBuffer& b) noexcept {
			return b.m_allocation;
		}

	};

}