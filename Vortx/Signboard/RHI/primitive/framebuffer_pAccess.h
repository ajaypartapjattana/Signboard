#pragma once

#include "framebuffer.h"

namespace rhi::access {

	struct framebuffer_pAccess {

		static VkFramebuffer get(const rhi::pmvFramebuffer& fb) noexcept {
			return fb.m_framebuffer;
		}

		static VkExtent2D get_extent(const rhi::pmvFramebuffer& fb) noexcept {
			return fb.m_extent;
		}

	};

}