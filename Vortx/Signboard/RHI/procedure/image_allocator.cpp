#include "image_allocator.h"

#include "Signboard/RHI/primitive/image.h"
#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/core/allocator_vma.h"

#include <stdexcept>

namespace rhi::procedure {

	image_allocator::image_allocator(const rhi::core::device& device, const rhi::core::allocator& allocator)
		: m_device(rhi::core::device_vkAccess::get(device)), m_allocator(rhi::core::allocator_vmaAccess::get(allocator))
	{
		reset_allocatorState();
	}

	image_allocator& image_allocator::set_usage_colorAttachment() {
		final_aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		return set_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	}

	image_allocator& image_allocator::set_usage_depthStencil() {
		final_aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
		return set_usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	image_allocator& image_allocator::set_usage_sampled() {
		return set_usage(VK_IMAGE_USAGE_SAMPLED_BIT);
	}

	image_allocator& image_allocator::set_usage_transferDst() {
		return set_usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	}

	image_allocator& image_allocator::set_format_R8G8B8A8() {
		final_aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		return set_format(VK_FORMAT_R8G8B8A8_UNORM);
	}

	image_allocator& image_allocator::set_extent(uint32_t w, uint32_t h) {
		final_extent = { w, h, 1 };
		return *this;
	}

	image_allocator& image_allocator::set_usage(VkImageUsageFlags usage) {
		final_usage |= usage;
		return *this;
	}

	image_allocator& image_allocator::set_format(VkFormat format) {
		final_format = format;
		return *this;
	}

	rhi::primitive::image image_allocator::allocate() {
		if (final_format == VK_FORMAT_UNDEFINED)
			throw std::runtime_error("UNDEFINED: image_format!");

		if (final_usage == 0)
			throw std::runtime_error("UNDEFINED: image_usage!");

		if (final_extent.width == 0 || final_extent.height == 0)
			throw std::runtime_error("INVALID: image_extent!");

		if (final_aspect == 0)
			throw std::runtime_error("INVALID: image_aspect!");

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = final_format;
		imageInfo.extent = final_extent;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = final_usage;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		VkImage vk_image = VK_NULL_HANDLE;
		VmaAllocation vma_allocation = VK_NULL_HANDLE;
		if (vmaCreateImage(m_allocator, &imageInfo, &allocInfo, &vk_image, &vma_allocation, nullptr) != VK_SUCCESS)
			throw std::runtime_error("FAILED: vma_image_allocation!");

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = vk_image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = final_format;
		viewInfo.subresourceRange.aspectMask = final_aspect;
		viewInfo.subresourceRange.levelCount = imageInfo.mipLevels;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		
		VkImageView vk_view = VK_NULL_HANDLE;
		if (vkCreateImageView(m_device, &viewInfo, nullptr, &vk_view) != VK_SUCCESS) {
			vmaDestroyImage(m_allocator, vk_image, vma_allocation);
			throw std::runtime_error("FAILED: image_viewCreation!");
		}

		rhi::primitive::image l_image;
		l_image.m_image = vk_image;
		l_image.m_allocation = vma_allocation;
		l_image.m_view = vk_view;
		l_image.m_device = m_device;
		l_image.m_allocator = m_allocator;

		reset_allocatorState();

		return l_image;
	}

	void image_allocator::reset_allocatorState() {
		final_usage = 0;
		final_format = VK_FORMAT_UNDEFINED;
		final_extent = { 0, 0, 1 };
		final_aspect = 0;
	}

}