#pragma once

#include "vulkan_trait_base.hh"

namespace rhi {

	struct pipelineLayout : vulkan_trait_base<pipelineLayout> {
		using handle_type = VkPipelineLayout;
		using parent_type = VkDevice;
		using createInfo_type = VkPipelineLayoutCreateInfo;
		using result_type = VkResult;

		static result_type create(parent_type device, const createInfo_type* pInfo, handle_type* pPipelineLayout) noexcept {
			return vkCreatePipelineLayout(device, pInfo, nullptr, pPipelineLayout);
		}

		static void destroy(parent_type device, handle_type pipelineLayout) noexcept {
			if (!pipelineLayout)
				return;

			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		}
	};

}