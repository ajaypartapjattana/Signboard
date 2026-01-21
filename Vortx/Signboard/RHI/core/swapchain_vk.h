#pragma once

#include <vulkan/vulkan.h>
#include "swapchain.h"

namespace rhi::core {

	struct swapchain_vkAccess {

		static VkSwapchainKHR get(const swapchain& s) {
			return reinterpret_cast<VkSwapchainKHR>(s.m_swapchain);
		}

		static VkAllocationCallbacks* allocator(const swapchain& s) {
			return reinterpret_cast<VkAllocationCallbacks*>(s.m_allocator);
		}

	};

}