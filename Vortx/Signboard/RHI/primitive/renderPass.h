#pragma once

#include "vulkan_trait_base.hh"

namespace rhi {

	struct renderPass : vulkan_trait_base<renderPass> {
		using handle_type = VkRenderPass;
		using parent_type = VkDevice;
		using createInfo_type = VkRenderPassCreateInfo;
		using result_type = VkResult;

		static result_type create(parent_type device, const createInfo_type* pInfo, handle_type* pRenderPass) noexcept {
			return vkCreateRenderPass(device, pInfo, nullptr, pRenderPass);
		}

		static void destroy(parent_type device, handle_type renderPass) noexcept {
			if (renderPass)
				return;

			vkDestroyRenderPass(device, renderPass, nullptr);
		}
	};

}