#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "Signboard/Core/core.h"

namespace rhi {

	class creDevice;

	class pmvFramebuffer;

	class pmvRenderPass;
	class pmvImage;

	class pcdFramebufferCreate {
	public:
		pcdFramebufferCreate(const creDevice& device, VkFramebufferCreateInfo* pCreateInfo = nullptr) noexcept;

		pcdFramebufferCreate(const pcdFramebufferCreate&) = delete;
		pcdFramebufferCreate& operator=(const pcdFramebufferCreate&) = delete;

		VkResult target_renderPass(const pmvRenderPass& renderPass) noexcept;
		VkResult target_attachments(sgb::span<const pmvImage*> images) noexcept;

		VkResult publish(pmvFramebuffer& target_framebuffer);

		void preset(VkFramebufferCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkFramebufferCreateInfo* fetch_basic(VkFramebufferCreateInfo* pCreateInfo) noexcept;
		
	private:
		const VkDevice r_device;

		uint32_t renderPassAttachmentSlots;
		std::vector<VkImageView> m_attachmentImageViews;
		
		std::unique_ptr<VkFramebufferCreateInfo> m_ownedInfo;
		VkFramebufferCreateInfo* pInfo;

	};

}