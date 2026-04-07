#include "imageAllocator.h"

#include "Signboard/RHI/primitive/image.h"
#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/core/allocator_pAccess.h"

namespace rhi {

	pcdImageAllocator::pcdImageAllocator(const rhi::creDevice& device, const rhi::creAllocator& allocator)
		: 
		_dvc(rhi::access::device_pAccess::get(device)), 
		_allctr(rhi::access::allocator_pAccess::get(allocator))
	{

	}

	pcdImageAllocator& pcdImageAllocator::set_usage(VkImageUsageFlags usage) noexcept {
		final_usage |= usage;
		return *this;
	}

	pcdImageAllocator& pcdImageAllocator::set_format(VkFormat format) noexcept {
		final_format = format;
		return *this;
	}

	pcdImageAllocator& pcdImageAllocator::set_aspect(VkImageAspectFlags aspect) noexcept {
		final_aspect = aspect;
		return *this;
	}

	pcdImageAllocator& pcdImageAllocator::set_extent(VkExtent2D extent) noexcept {
		final_extent = { extent.width, extent.height, 1 };
		return *this;
	}

	pcdImageAllocator& pcdImageAllocator::set_extent(VkExtent3D extent) noexcept {
		final_extent = extent;
		return *this;
	}

	VkResult pcdImageAllocator::allocate(rhi::pmvImage& target_image) const {
		if (final_format == VK_FORMAT_UNDEFINED ||
			final_usage == 0 ||
			(final_extent.width == 0 || final_extent.height == 0) ||
			final_aspect == 0)
			return VK_INCOMPLETE;

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
		VkResult result = vmaCreateImage(_allctr, &imageInfo, &allocInfo, &vk_image, &vma_allocation, nullptr);

		if (result != VK_SUCCESS)
			return result;

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
		result = vkCreateImageView(_dvc, &viewInfo, nullptr, &vk_view);

		if (result != VK_SUCCESS) {
			vmaDestroyImage(_allctr, vk_image, vma_allocation);
			return result;
		}

		target_image.m_image = vk_image;
		target_image._alloc = vma_allocation;
		target_image.m_view = vk_view;

		target_image.m_extent = final_extent;

		target_image._dvc = _dvc;
		target_image._allctr = _allctr;

		return result;
	}

}