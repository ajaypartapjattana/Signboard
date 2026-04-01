#pragma once

#include <vulkan/vulkan.h>
#include "Signboard/Core/Containers/storage.h"

namespace rhi {

	class creDevice;

	class pmvFramebuffer;

	class pmvSwapchain;
	class pmvRenderPass;
	class pmvImage;

	class pcdFramebufferCreator {
	public:
		pcdFramebufferCreator(const rhi::creDevice& device) noexcept;

		pcdFramebufferCreator(const pcdFramebufferCreator&) = delete;
		pcdFramebufferCreator& operator=(const pcdFramebufferCreator&) = delete;

		pcdFramebufferCreator& bind_renderpass(const rhi::pmvRenderPass& pass) noexcept;
		
		pcdFramebufferCreator& add_attachament(const rhi::pmvImage& image);
		pcdFramebufferCreator& set_extent(const VkExtent3D extent) noexcept;
		pcdFramebufferCreator& set_swapchainExtent(const rhi::pmvSwapchain& swapchain) noexcept;

		VkResult create_offScreenTarget_framebuffer(rhi::pmvFramebuffer& target_framebuffer);
		VkResult create_swapchainTarget_framebuffer(const rhi::pmvSwapchain& swapchain, ctnr::vault_writeAccessor<rhi::pmvFramebuffer>& writer, std::vector<uint32_t>& fb_handles) const;

	private:
		VkDevice r_device;

		VkRenderPass r_toBindPass;
		VkExtent3D m_bufferExtent;

		std::vector<VkImageView> m_attachments;

	};

}