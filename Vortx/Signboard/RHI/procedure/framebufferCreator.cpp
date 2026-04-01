#include "framebufferCreator.h"

#include "Signboard/RHI/primitive/framebuffer.h"

#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/swapchain_pAccess.h"
#include "Signboard/RHI/primitive/renderPass_pAccess.h"
#include "Signboard/RHI/primitive/image_pAccess.h"

namespace rhi {

	pcdFramebufferCreator::pcdFramebufferCreator(const rhi::creDevice& device) noexcept
		: 
		r_device(rhi::access::device_pAccess::get(device)),
		r_toBindPass(VK_NULL_HANDLE),
		m_bufferExtent()
	{

	}

	pcdFramebufferCreator& pcdFramebufferCreator::bind_renderpass(const rhi::pmvRenderPass& pass) noexcept {
		r_toBindPass = rhi::access::renderPass_pAccess::get(pass);
		return *this;
	}

	pcdFramebufferCreator& pcdFramebufferCreator::add_attachament(const rhi::pmvImage& image) {
		VkImageView a_view = rhi::access::image_pAccess::get_view(image);
		m_attachments.push_back(a_view);
		return *this;
	}

	pcdFramebufferCreator& pcdFramebufferCreator::set_extent(const VkExtent3D extent) noexcept {
		m_bufferExtent = extent;
		return *this;
	}

	pcdFramebufferCreator& pcdFramebufferCreator::set_swapchainExtent(const rhi::pmvSwapchain& swapchain) noexcept {
		VkExtent2D a_swapchainExtent = rhi::access::swapchain_pAccess::get_extent(swapchain);
		m_bufferExtent.width = a_swapchainExtent.width;
		m_bufferExtent.height = a_swapchainExtent.height;
		m_bufferExtent.depth = 1;
		return *this;
	}

	VkResult pcdFramebufferCreator::create_offScreenTarget_framebuffer(rhi::pmvFramebuffer& target_framebuffer) {
		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = r_toBindPass;
		createInfo.attachmentCount = static_cast<uint32_t>(m_attachments.size());
		createInfo.pAttachments = m_attachments.data();
		createInfo.width = m_bufferExtent.width;
		createInfo.height = m_bufferExtent.height;
		createInfo.layers = m_bufferExtent.depth;

		VkResult result = vkCreateFramebuffer(r_device, &createInfo, nullptr, &target_framebuffer.m_framebuffer);
		if (result != VK_SUCCESS)
			return result;

		target_framebuffer.m_extent = { m_bufferExtent.width, m_bufferExtent.height };
		target_framebuffer.m_device = r_device;

		m_attachments.clear();
		return result;
	}

	VkResult pcdFramebufferCreator::create_swapchainTarget_framebuffer(const rhi::pmvSwapchain& swapchain, ctnr::vault_writeAccessor<rhi::pmvFramebuffer>& writer, std::vector<uint32_t>& fb_handles) const {	
		const std::vector<VkImageView>& a_swapchainViews = rhi::access::swapchain_pAccess::get_views(swapchain);
		const uint32_t swapchainView_count = static_cast<uint32_t>(a_swapchainViews.size());

		VkExtent2D a_swapchainExtent = rhi::access::swapchain_pAccess::get_extent(swapchain);
		
		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = r_toBindPass;
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
			rhi::pmvFramebuffer& a_framebuffer = *writer.get(fb_handles[i]);

			vkDestroyFramebuffer(r_device, a_framebuffer.m_framebuffer, nullptr);

			a_framebuffer.m_framebuffer = l_framebuffers[i];
			a_framebuffer.m_extent = a_swapchainExtent;
			a_framebuffer.m_device = r_device;
		}

		for (uint32_t i = newCount; i < oldCount; ++i) {
			rhi::pmvFramebuffer& a_framebuffer = *writer.get(fb_handles[i]);
			vkDestroyFramebuffer(r_device, a_framebuffer.m_framebuffer, nullptr);
			a_framebuffer.m_framebuffer = VK_NULL_HANDLE;
		}

		fb_handles.reserve(swapchainView_count);
		for (uint32_t i = oldCount; i < swapchainView_count; ++i) {
			auto builder = [vk_framebuffer = l_framebuffers[i], a_swapchainExtent, device = r_device](rhi::pmvFramebuffer* fb) {
				rhi::pmvFramebuffer& a_framebuffer = *fb;

				a_framebuffer.m_framebuffer = vk_framebuffer;
				a_framebuffer.m_extent = a_swapchainExtent;
				a_framebuffer.m_device = device;
			};

			fb_handles.push_back(writer.construct(builder));
		}

		return VK_SUCCESS;
	}


}