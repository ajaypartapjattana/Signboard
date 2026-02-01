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
		
	}

	rhi::primitive::renderPass renderPass_builder::build(bool keep_state) {

	}

	void renderPass_builder::reset_builder() {

	}

}