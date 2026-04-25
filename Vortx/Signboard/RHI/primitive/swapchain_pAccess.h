#pragma once

#include "swapchain.h"

namespace rhi::access {

	struct swapchain_pAccess {

		static VkSwapchainKHR extract(const pmvSwapchain& s) noexcept{
			return s.m_swapchain;
		}

		static VkFormat get_format(const pmvSwapchain& s) noexcept{
			return s.format;
		}

		static VkExtent2D get_extent(const pmvSwapchain& s) noexcept {
			return s.extent;
		}

	};

}