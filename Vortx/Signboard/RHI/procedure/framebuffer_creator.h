#pragma once

namespace rhi::core {
	class device;
}

namespace rhi::primitive {
	class framebuffer;
	class swapchain;
	class renderPass;
	class image;
}

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::procedure {

	class framebuffer_creator {
	public:
		framebuffer_creator(const rhi::core::device& device) noexcept;

		framebuffer_creator(const framebuffer_creator&) = delete;
		framebuffer_creator& operator=(const framebuffer_creator&) = delete;

		framebuffer_creator& bind_renderpass(const rhi::primitive::renderPass& pass) noexcept;
		
		framebuffer_creator& add_attachament(const rhi::primitive::image& image);
		framebuffer_creator& set_extent(const VkExtent3D extent) noexcept;
		framebuffer_creator& set_swapchainExtent(const rhi::primitive::swapchain& swapchain) noexcept;

		VkResult create_offScreenTarget_framebuffer(rhi::primitive::framebuffer& target_framebuffer);
		VkResult create_swapchainTarget_framebuffer(const rhi::primitive::swapchain& swapchain, uint32_t* framebuffer_count , rhi::primitive::framebuffer* target_framebuffer) const;

	private:
		VkDevice am_device;

		VkRenderPass rm_toBindPass = VK_NULL_HANDLE;
		VkExtent3D am_bufferExtent{};

		std::vector<VkImageView> m_attachments;

	};

}