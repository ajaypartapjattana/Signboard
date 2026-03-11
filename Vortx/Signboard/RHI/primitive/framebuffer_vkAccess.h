#pragma once

#include "framebuffer.h"

namespace rhi::primitive {

	struct framebuffer_vkAccess {

		static VkFramebuffer get(const framebuffer& fb) noexcept {
			return fb.m_framebuffer;
		}

		static VkRenderPass get_nativePass(const framebuffer& fb) noexcept {
			return fb.m_nativePass;
		}

		static VkExtent2D get_extent(const framebuffer& fb) noexcept {
			return fb.m_extent;
		}

	};

}