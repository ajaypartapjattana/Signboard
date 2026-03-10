#include "framebuffer_creator.h"

#include "Signboard/RHI/primitive/framebuffer.h"

#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/primitive/swapchain_vk.h"
#include "Signboard/RHI/primitive/renderPass_vk.h"
#include "Signboard/RHI/primitive/image_vk.h"

namespace rhi::procedure {

	framebuffer_creator::framebuffer_creator(const rhi::core::device& device) noexcept
		: am_device(rhi::core::device_vkAccess::get(device)), rm_toBindPass(VK_NULL_HANDLE)
	{

	}

	framebuffer_creator& framebuffer_creator::bind_renderpass(const rhi::primitive::renderPass& pass) noexcept {
		rm_toBindPass = rhi::primitive::renderPass_vkAccess::get(pass);
		return *this;
	}

	framebuffer_creator& framebuffer_creator::add_attachament(const rhi::primitive::image& image) {
		VkImageView a_view = rhi::primitive::image_vkAccess::get_view(image);
		m_attachments.push_back(a_view);
		return *this;
	}

	framebuffer_creator& framebuffer_creator::set_extent(const VkExtent3D extent) noexcept {
		am_bufferExtent = extent;
		return *this;
	}

	framebuffer_creator& framebuffer_creator::set_swapchainExtent(const rhi::primitive::swapchain& swapchain) noexcept {
		VkExtent2D a_swapchainExtent = rhi::primitive::swapchain_vkAccess::get_extent(swapchain);
		am_bufferExtent.width = a_swapchainExtent.width;
		am_bufferExtent.height = a_swapchainExtent.height;
		am_bufferExtent.depth = 1;
		return *this;
	}

	VkResult framebuffer_creator::create_offScreenTarget_framebuffer(rhi::primitive::framebuffer& target_framebuffer) {
		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = rm_toBindPass;
		createInfo.attachmentCount = static_cast<uint32_t>(m_attachments.size());
		createInfo.pAttachments = m_attachments.data();
		createInfo.width = am_bufferExtent.width;
		createInfo.height = am_bufferExtent.height;
		createInfo.layers = am_bufferExtent.depth;

		VkResult result = vkCreateFramebuffer(am_device, &createInfo, nullptr, &target_framebuffer.m_framebuffer);

		m_attachments.clear();
		return result;
	}

	VkResult framebuffer_creator::create_swapchainTarget_framebuffer(const rhi::primitive::swapchain& swapchain, uint32_t* framebuffer_count, rhi::primitive::framebuffer* target_framebuffer) const {
		if (!framebuffer_count)
			return VK_ERROR_INITIALIZATION_FAILED;
		
		const std::vector<VkImageView>& a_swapchainViews = rhi::primitive::swapchain_vkAccess::get_views(swapchain);
		const uint32_t swapchainView_count = static_cast<uint32_t>(a_swapchainViews.size());
		if (*framebuffer_count != swapchainView_count) {
			*framebuffer_count = swapchainView_count;
			return VK_INCOMPLETE;
		}

		VkExtent2D a_swapchainExtent = rhi::primitive::swapchain_vkAccess::get_extent(swapchain);
		
		for (uint32_t i = 0; i < swapchainView_count; i++) {
			VkImageView attachment[] = { a_swapchainViews[i]};

			VkFramebufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.renderPass = rm_toBindPass;
			createInfo.attachmentCount = 1;
			createInfo.pAttachments = attachment;
			createInfo.width = a_swapchainExtent.width;
			createInfo.height = a_swapchainExtent.height;
			createInfo.layers = 1;

			VkResult result = vkCreateFramebuffer(am_device, &createInfo, nullptr, &target_framebuffer[i].m_framebuffer);

			if (result != VK_SUCCESS) {
				for (uint32_t j = 0; j < i; j++) {
					vkDestroyFramebuffer(am_device, target_framebuffer[j].m_framebuffer, nullptr);
					target_framebuffer[j].m_framebuffer = VK_NULL_HANDLE;
				}
				return result;
			}
		}

		return VK_SUCCESS;
	}


}