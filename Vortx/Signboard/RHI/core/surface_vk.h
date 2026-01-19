#pragma once

#include <vulkan/vulkan.h>
#include "surface.h"

namespace rhi::core {

	struct surface_vkAccess {

		static VkSurfaceKHR get(const surface& s) noexcept {
			return reinterpret_cast<VkSurfaceKHR>(s.m_surface);
		}

		static VkAllocationCallbacks* allocator(const surface& s) noexcept {
			return reinterpret_cast<VkAllocationCallbacks*>(s.m_allocator);
		}

	};

}