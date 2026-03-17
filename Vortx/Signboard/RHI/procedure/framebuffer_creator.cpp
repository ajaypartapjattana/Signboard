#include "framebuffer_creator.h"

#include "Signboard/RHI/primitive/framebuffer.h"

#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/primitive/swapchain_vk.h"
#include "Signboard/RHI/primitive/renderPass_vk.h"
#include "Signboard/RHI/primitive/image_vk.h"

namespace rhi::procedure {

	framebuffer_creator::framebuffer_creator(const rhi::core::device& device) noexcept
		: r_device(rhi::core::device_vkAccess::get(device)), rm_toBindPass(VK_NULL_HANDLE)
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

		VkResult result = vkCreateFramebuffer(r_device, &createInfo, nullptr, &target_framebuffer.m_framebuffer);
		if (result != VK_SUCCESS)
			return result;

		target_framebuffer.m_extent = { am_bufferExtent.width, am_bufferExtent.height };
		target_framebuffer.m_device = r_device;

		m_attachments.clear();
		return result;
	}

	VkResult framebuffer_creator::create_swapchainTarget_framebuffer(const rhi::primitive::swapchain& swapchain, storage::vault_writeAccessor<rhi::primitive::framebuffer>& writer, std::vector<storage::storage_handle>& fb_handles) const {	
		const std::vector<VkImageView>& a_swapchainViews = rhi::primitive::swapchain_vkAccess::get_views(swapchain);
		const uint32_t swapchainView_count = static_cast<uint32_t>(a_swapchainViews.size());

		VkExtent2D a_swapchainExtent = rhi::primitive::swapchain_vkAccess::get_extent(swapchain);
		
		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = rm_toBindPass;
		createInfo.attachmentCount = 1;
		createInfo.width = a_swapchainExtent.width;
		createInfo.height = a_swapchainExtent.height;
		createInfo.layers = 1;

		std::vector<VkFramebuffer> l_framebuffers(swapchainView_count, VK_NULL_HANDLE);
		for (uint32_t i = 0; i < swapchainView_count; i++) {
			VkImageView attachment[] = { a_swapchainViews[i]};
			createInfo.pAttachments = attachment;

			VkResult result = vkCreateFramebuffer(r_device, &createInfo, nullptr, &l_framebuffers[i]);

			if (result != VK_SUCCESS) {
				for (uint32_t j = 0; j < i; j++) {
					vkDestroyFramebuffer(r_device, l_framebuffers[j], nullptr);
					l_framebuffers[j] = VK_NULL_HANDLE;
				}
				return result;
			}
		}

		uint32_t oldCount = static_cast<uint32_t>(fb_handles.size());
		uint32_t newCount = swapchainView_count;

		uint32_t common = std::min(oldCount, newCount);

		for (uint32_t i = 0; i < common; ++i) {
			rhi::primitive::framebuffer& a_framebuffer = *writer.get(fb_handles[i]);

			vkDestroyFramebuffer(r_device, a_framebuffer.m_framebuffer, nullptr);

			a_framebuffer.m_framebuffer = l_framebuffers[i];
			a_framebuffer.m_extent = a_swapchainExtent;
			a_framebuffer.m_device = r_device;
		}

		for (uint32_t i = newCount; i < oldCount; ++i) {
			rhi::primitive::framebuffer& a_framebuffer = *writer.get(fb_handles[i]);
			vkDestroyFramebuffer(r_device, a_framebuffer.m_framebuffer, nullptr);
			a_framebuffer.m_framebuffer = VK_NULL_HANDLE;
		}

		fb_handles.reserve(swapchainView_count);
		for (uint32_t i = oldCount; i < swapchainView_count; ++i) {
			auto builder = [vk_framebuffer = l_framebuffers[i], a_swapchainExtent, device = r_device](rhi::primitive::framebuffer* fb) {
				rhi::primitive::framebuffer& a_framebuffer = *fb;

				a_framebuffer.m_framebuffer = vk_framebuffer;
				a_framebuffer.m_extent = a_swapchainExtent;
				a_framebuffer.m_device = device;
			};

			fb_handles.push_back(writer.construct(builder));
		}

		return VK_SUCCESS;
	}


}