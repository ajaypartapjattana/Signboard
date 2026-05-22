#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	struct fence_traits {
		using createInfo = VkFenceCreateInfo;

		static VkResult create(VkDevice device, createInfo* pInfo, VkFence* pFence) noexcept {
			return vkCreateFence(device, pInfo, nullptr, pFence);
		}

		static void destroy(VkDevice device, VkFence fence) noexcept {
			if (!fence)
				return;

			vkDestroyFence(device, fence, nullptr);
		}
	};

}