#pragma once

#include "vulkan_trait_base.hh"

namespace rhi {
	
	struct shaderModule {
		using handle_type = VkShaderModule;
		using parent_type = VkDevice;
		using createInfo_type = VkShaderModuleCreateInfo;
		using result_type = VkResult;

		static result_type create(parent_type device, const createInfo_type* pInfo, handle_type* pShaderModule) noexcept {
			return vkCreateShaderModule(device, pInfo, nullptr, pShaderModule);
		}

		static void destroy(parent_type device, handle_type shaderModule) noexcept {
			if (!shaderModule)
				return;

			vkDestroyShaderModule(device, shaderModule, nullptr);
		}
	};

}