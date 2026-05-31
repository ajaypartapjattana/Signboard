#pragma once

#include "vulkan_trait_base.hh"

namespace rhi {

	struct descriptorSetLayout : vulkan_trait_base<descriptorSetLayout> {
		using handle_type = VkDescriptorSetLayout;
		using parent_type = VkDevice;
		using createInfo_type = VkDescriptorSetLayoutCreateInfo;
		using result_type = VkResult;

		static result_type create(parent_type device, const createInfo_type* pInfo, handle_type* pDescriptorSetLayout) noexcept {
			return vkCreateDescriptorSetLayout(device, pInfo, nullptr, pDescriptorSetLayout);
		}

		static void destroy(parent_type device, handle_type descriptorSetLayout) noexcept {
			if (!descriptorSetLayout)
				return;

			vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		}
	};

}