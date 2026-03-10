#pragma once

#include "swapchain.h"

namespace rhi::primitive {

	struct swapchain_vkAccess {

		static VkSwapchainKHR get(const swapchain& s) noexcept{
			return s.m_swapchain;
		}

		static VkFormat get_format(const swapchain& s) noexcept{
			return s.m_format;
		}

		static VkExtent2D get_extent(const swapchain& s) noexcept {
			return s.m_extent;
		}

		static const std::vector<VkImageView>& get_views(const swapchain& s) noexcept {
			return s.m_views;
		}

	};

}