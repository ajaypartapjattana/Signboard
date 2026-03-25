#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	class creDevice;

	class pmvRenderPass;
	class pmvImage;

	class pcdRenderPassBuilder {
	public:
		pcdRenderPassBuilder(const rhi::creDevice& device);

		pcdRenderPassBuilder(const pcdRenderPassBuilder&) = delete;
		pcdRenderPassBuilder& operator=(const pcdRenderPassBuilder&) = delete;

		struct attachment_desc {
			VkFormat format = VK_FORMAT_UNDEFINED;
			VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			VkImageLayout finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			VkImageLayout usageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		};

		pcdRenderPassBuilder& add_colorAttachment(const rhi::pmvImage* image, const attachment_desc& desc);

		VkResult build_graphicsPass(rhi::pmvRenderPass& targetPass);

	private:
		VkDevice m_device;

		std::vector<VkAttachmentDescription> m_attachments;
		std::vector<VkAttachmentReference> m_attachmentRef;

	};

}