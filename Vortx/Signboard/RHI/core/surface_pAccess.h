#pragma once

#include "surface.h"

namespace rhi::access {

	struct surface_pAccess {

		static VkSurfaceKHR extract(const rhi::creSurface& s) noexcept {
			return s.m_surface;
		}

	};

}