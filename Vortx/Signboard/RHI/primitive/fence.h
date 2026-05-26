#pragma once

#include "vulkan_trait_base.hh"

namespace rhi {

	struct fence : vulkan_trait_base<fence> {
		using handle_type = VkFence;
		using parent_type = VkDevice;
		using createInfo_type = VkFenceCreateInfo;
		using result_type = VkResult;

		static result_type create(parent_type device, const createInfo_type* pInfo, handle_type* pFence) noexcept {
			return vkCreateFence(device, pInfo, nullptr, pFence);
		}

		static void destroy(parent_type device, handle_type fence) noexcept {
			if (!fence)
				return;

			vkDestroyFence(device, fence, nullptr);
		}
	};

}