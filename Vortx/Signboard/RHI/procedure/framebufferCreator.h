#pragma once

#include <vulkan/vulkan.h>
#include "Signboard/Core/Containers/storage.h"

namespace rhi {

	class creDevice;

	class pmvFramebuffer;

	class pmvRenderPass;
	class pmvImage;

	class pcdFramebufferCreate {
	public:
		pcdFramebufferCreate(const rhi::creDevice& device, const rhi::pmvRenderPass& renderPass, VkFramebufferCreateInfo* pCreateInfo = nullptr) noexcept;

		pcdFramebufferCreate(const pcdFramebufferCreate&) = delete;
		pcdFramebufferCreate& operator=(const pcdFramebufferCreate&) = delete;

		VkResult push_attachments(ctnr::span<const rhi::pmvImage> images);

		VkResult publish(rhi::pmvFramebuffer& target_framebuffer);

		void preset(VkFramebufferCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkFramebufferCreateInfo fetch_basic(VkFramebufferCreateInfo* pCreateInfo) const noexcept;
		
	private:
		VkDevice r_device;
		VkRenderPass r_renderPass;
		ctnr::span<const VkFormat> renderPass_attachmentFormats;

		std::vector<VkImageView> m_attachmentImageViews;
		
		VkFramebufferCreateInfo _info;

	};

}