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

		renderPass_builder& add_attachment(const rhi::primitive::image& image);

		VkResult build(rhi::primitive::renderPass& targetPass);

	private:
		VkDevice m_device = VK_NULL_HANDLE;

		std::vector<VkAttachmentDescription> m_attachments;

	};

}