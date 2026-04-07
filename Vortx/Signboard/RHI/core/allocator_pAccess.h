#pragma once

#include "allocator.h"

namespace rhi::access {

	struct allocator_pAccess {

		static VmaAllocator get(const rhi::creAllocator& a) noexcept {
			return a._allctr;
		}

		static const VkPhysicalDeviceMemoryProperties& get_memProps(const rhi::creAllocator& a) noexcept {
			return a._memProps;
		}

	};

}