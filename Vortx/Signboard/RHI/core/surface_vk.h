#pragma once

#include "surface.h"

namespace rhi::core {

	struct surface_vkAccess {

		static VkSurfaceKHR get(const surface& s) noexcept {
			return s.m_surface;
		}

		static VkAllocationCallbacks* allocator(const surface& s) noexcept {
			return s.m_allocator;
		}

	};

}