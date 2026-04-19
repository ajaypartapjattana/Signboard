#include "framebufferCreator.h"

#include "Signboard/RHI/primitive/framebuffer.h"

#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/swapchain_pAccess.h"
#include "Signboard/RHI/primitive/renderPass_pAccess.h"
#include "Signboard/RHI/primitive/image_pAccess.h"

namespace rhi {

	pcdFramebufferCreate::pcdFramebufferCreate(const rhi::creDevice& device, const rhi::pmvRenderPass& renderPass, VkFramebufferCreateInfo* pCreateInfo) noexcept
		: 
		r_device(rhi::access::device_pAccess::get(device)),
		r_renderPass(rhi::access::renderPass_pAccess::extract(renderPass)),
		renderPass_attachmentFormats(rhi::access::renderPass_pAccess::get_orderedAttachmentFormats(renderPass)),
		_info(fetch_basic(pCreateInfo))
	{

	}

	VkFramebufferCreateInfo pcdFramebufferCreate::fetch_basic(VkFramebufferCreateInfo* pCreateInfo) const noexcept {
		if (pCreateInfo)
			return *pCreateInfo;

		VkFramebufferCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = r_renderPass;
		info.layers = 1;

		return info;
	}

	VkResult pcdFramebufferCreate::push_attachments(ctnr::span<const rhi::pmvImage> images) {
		if (images.size() != renderPass_attachmentFormats.size())
			return VK_INCOMPLETE;

		uint32_t _ISz = static_cast<uint32_t>(images.size());

		VkExtent3D extent_t = rhi::access::image_pAccess::get_extent(images[0]);
		for (uint32_t i = 0; i < _ISz; ++i) {
			VkFormat attachmentFormat = rhi::access::image_pAccess::get_format(images[i]);
			if (attachmentFormat != renderPass_attachmentFormats[i]) {
				return VK_INCOMPLETE;
			}
			VkExtent3D attachmentExtent = rhi::access::image_pAccess::get_extent(images[i]);
			if (attachmentExtent.width != extent_t.width || attachmentExtent.height != extent_t.height)
				return VK_INCOMPLETE;
		}

		m_attachmentImageViews.clear();
		m_attachmentImageViews.reserve(_ISz);
		for (const rhi::pmvImage& image : images) {
			m_attachmentImageViews.push_back(rhi::access::image_pAccess::get_view(image));
		}

		_info.width = extent_t.width;
		_info.height = extent_t.height;
		_info.attachmentCount = static_cast<uint32_t>(m_attachmentImageViews.size());
		_info.pAttachments = m_attachmentImageViews.data();

		return VK_SUCCESS;
	}

	VkResult pcdFramebufferCreate::publish(rhi::pmvFramebuffer& target) {
		if (m_attachmentImageViews.empty())
			return VK_INCOMPLETE;

		VkResult result = vkCreateFramebuffer(r_device, &_info, nullptr, &target.m_framebuffer);
		if (result != VK_SUCCESS)
			return result;

		target.extent = { _info.width, _info.height };
		target.r_device = r_device;

		return result;
	}

	void pcdFramebufferCreate::preset(VkFramebufferCreateInfo* pCreateInfo) noexcept {
		_info = fetch_basic(pCreateInfo);
	}

	void pcdFramebufferCreate::reset() noexcept {
		m_attachmentImageViews = {};
		_info = fetch_basic(nullptr);
	}

	/*VkResult pcdFramebufferCreate::create_swapchainTarget_framebuffer(const rhi::pmvSwapchain& swapchain, ctnr::vault_writeAccessor<rhi::pmvFramebuffer>& writer, std::vector<uint32_t>& fb_handles) const {	
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
			a_framebuffer.extent = a_swapchainExtent;
			a_framebuffer.r_device = r_device;
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
				a_framebuffer.extent = a_swapchainExtent;
				a_framebuffer.r_device = device;
			};

			fb_handles.push_back(writer.construct(builder));
		}

		return VK_SUCCESS;
	}*/


}