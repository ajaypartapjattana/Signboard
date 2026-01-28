#include "image_wrapper.h"

#include "Signboard/RHI/primitive/image.h"
#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/core/swapchain_vk.h"


namespace rhi::procedure {

	image_wrapper::image_wrapper(const rhi::core::device& device)
		: m_device(rhi::core::device_vkAccess::get(device))
	{

	}

	void image_wrapper::wrap_swapchainImages(const rhi::core::swapchain& swapchain, std::vector<rhi::primitive::image>& images) {
		VkSwapchainKHR a_swapchain = rhi::core::swapchain_vkAccess::get(swapchain);
		VkFormat a_format = rhi::core::swapchain_vkAccess::get_format(swapchain);

		uint32_t count = 0;
		vkGetSwapchainImagesKHR(m_device, a_swapchain, &count, nullptr);

		std::vector<VkImage> sc_images(count);
		vkGetSwapchainImagesKHR(m_device, a_swapchain, &count, sc_images.data());

		images.clear();
		images.reserve(count);

		for (VkImage image : sc_images) {
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = a_format;

			viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			VkImageView vk_view = VK_NULL_HANDLE;
			if (vkCreateImageView(m_device, &viewInfo, nullptr, &vk_view) != VK_SUCCESS)
				throw std::runtime_error("FAILED: swapchain_imageWrap!");

			rhi::primitive::image l_image;
			l_image.m_image = image;
			l_image.m_allocation = VK_NULL_HANDLE;
			l_image.m_view = vk_view;

			l_image.m_device = m_device;
			l_image.m_allocator = VK_NULL_HANDLE;

			images.emplace_back(std::move(l_image));

		}

	}

}