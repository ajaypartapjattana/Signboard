#include "renderPass_builder.h"

#include "Signboard/RHI/primitive/renderPass.h"
#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/primitive/image_vk.h"

#include <stdexcept>

namespace rhi::procedure {

	renderPass_builder::renderPass_builder(const rhi::core::device& device)
		: m_device(rhi::core::device_vkAccess::get(device))
	{

	}

	renderPass_builder& renderPass_builder::add_attachment(const rhi::primitive::image& image) {
		VkAttachmentDescription attachment{};
		attachment.format = rhi::primitive::image_vkAccess::get_format(image);
		
		return *this;
	}

	rhi::primitive::renderPass renderPass_builder::build(bool keep_state) {

		VkRenderPass vk_renderPass = VK_NULL_HANDLE;

		rhi::primitive::renderPass l_renderPass;
		l_renderPass.m_device = m_device;
		l_renderPass.m_renderPass = vk_renderPass;

		return l_renderPass;
	}

	void renderPass_builder::reset_builder() {

	}

}