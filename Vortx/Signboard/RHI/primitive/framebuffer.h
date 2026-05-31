#pragma once

#include "vulkan_trait_base.hh"
	
namespace rhi {

	struct framebuffer : vulkan_trait_base<framebuffer> {
		using handle_type = VkFramebuffer;
		using parent_type = VkDevice;
		using createInfo_type = VkFramebufferCreateInfo;
		using result_type = VkResult;

		static result_type create(parent_type device, const createInfo_type* pInfo, handle_type* pFramebuffer) noexcept {
			return vkCreateFramebuffer(device, pInfo, nullptr, pFramebuffer);
		}

		static void destroy(parent_type device, handle_type framebuffer) noexcept {
			if (!framebuffer)
				return;

			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}
	};

}