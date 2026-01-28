#pragma once

#include "swapchain.h"

namespace rhi::core {

	struct swapchain_vkAccess {

		static VkSwapchainKHR get(const swapchain& s) {
			return s.m_swapchain;
		}

		static VkFormat get_format(const swapchain& s) {
			return s.m_format;
		}

	};

}