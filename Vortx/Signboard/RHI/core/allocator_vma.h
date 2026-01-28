#pragma once

#include "allocator.h"

namespace rhi::core {

	struct allocator_vmaAccess {

		static VmaAllocator get(const allocator& a) noexcept {
			return a.m_allocator;
		}

	};

}