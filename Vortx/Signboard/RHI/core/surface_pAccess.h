#pragma once

#include "surface.h"

namespace rhi::access {

	struct surface_pAccess {

		static VkSurfaceKHR get(const rhi::creSurface& s) noexcept {
			return s.m_surface;
		}

	};

}