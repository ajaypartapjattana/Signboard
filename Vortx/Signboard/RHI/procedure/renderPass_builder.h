#pragma once

namespace rhi::core {
	class device;
}

namespace rhi::primitive {
	class renderPass;
	class image;
}

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::procedure {

	class renderPass_builder {
	public:
		renderPass_builder(const rhi::core::device& device);

		renderPass_builder(const renderPass_builder&) = delete;
		renderPass_builder& operator=(const renderPass_builder&) = delete;

		struct attachment_desc {
			VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			VkImageLayout finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		};

		renderPass_builder& add_colorAttachment(const rhi::primitive::image& image, const attachment_desc& desc);

		VkResult build_graphicsPass(rhi::primitive::renderPass& targetPass);

	private:
		VkDevice m_device = VK_NULL_HANDLE;

		std::vector<VkAttachmentDescription> m_attachments;
		std::vector<VkAttachmentReference> m_attachmentRef;

	};

}