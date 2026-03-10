#pragma once

#include "framebuffer.h"

namespace rhi::primitive {

	struct framebuffer_vkAccess {

		static VkFramebuffer get(const framebuffer& fb) noexcept {
			return fb.m_framebuffer;
		}

	};

}