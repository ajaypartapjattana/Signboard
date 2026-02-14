#pragma once

namespace rhi::core {
	class device;
	class swapchain;
}

namespace rhi::primitive {
	class image;
}

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::procedure {

	class image_wrapper {
	public:
		image_wrapper(const rhi::core::device& device) noexcept;

		VkResult wrap_swapchainImages(const rhi::core::swapchain& swapchain, std::vector<rhi::primitive::image>& target_images);

	private:
		VkDevice m_device;

	};

}