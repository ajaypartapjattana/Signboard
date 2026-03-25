#pragma once

#include "swapchain.h"

namespace rhi::access {

	struct swapchain_pAccess {

		static VkSwapchainKHR get(const pmvSwapchain& s) noexcept{
			return s.m_swapchain;
		}

		static VkFormat get_format(const pmvSwapchain& s) noexcept{
			return s.m_format;
		}

		static VkExtent2D get_extent(const pmvSwapchain& s) noexcept {
			return s.m_extent;
		}

		static const std::vector<VkImageView>& get_views(const pmvSwapchain& s) noexcept {
			return s.m_views;
		}

	};

}