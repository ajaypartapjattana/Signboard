#pragma once

#include "vulkan_trait_base.hh"

namespace rhi {

	struct graphicsPipeline : vulkan_trait_base<graphicsPipeline> {
		using handle_type = VkPipeline;
		using parent_type = VkDevice;
		using createInfo_type = VkGraphicsPipelineCreateInfo;
		using result_type = VkResult;

		static result_type create(parent_type device, const createInfo_type* pInfo, handle_type* pPipeline) noexcept {
			return vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, pInfo, nullptr, pPipeline);
		}

		static void destroy(parent_type device, handle_type pipeline) noexcept {
			if (!pipeline)
				return;

			vkDestroyPipeline(device, pipeline, nullptr);
		}
	};

}