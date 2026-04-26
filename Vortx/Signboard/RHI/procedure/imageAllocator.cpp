#include "imageAllocator.h"

#include "Signboard/RHI/primitive/image.h"
#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/core/allocator_pAccess.h"

namespace rhi {

	pcdImageAllocate::pcdImageAllocate(const rhi::creDevice& device, const rhi::creAllocator& allocator, const VkImageCreateInfo* pCreateInfo)
		: 
		r_device(rhi::access::device_pAccess::extract(device)), 
		r_allocator(rhi::access::allocator_pAccess::get(allocator)),

		_imageInfo(fetch_basicImageInfo(pCreateInfo)),
		_allocInfo()
	{

	}

	VkImageCreateInfo pcdImageAllocate::fetch_basicImageInfo(const VkImageCreateInfo* pCreateInfo) const noexcept {
		if (pCreateInfo)
			return *pCreateInfo;

		VkImageCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

		info.imageType = VK_IMAGE_TYPE_2D;
		info.mipLevels = 1;
		info.arrayLayers = 1;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
		return info;
	}

	void pcdImageAllocate::image_usage(VkImageUsageFlags usage) noexcept {
		_imageInfo.usage = usage;
	}

	void pcdImageAllocate::image_format(VkFormat format) noexcept {
		_imageInfo.format = format;
	}

	void pcdImageAllocate::image_extent(VkExtent3D extent) noexcept {
		_imageInfo.extent = extent;
	}

	void pcdImageAllocate::allocation_usage(VmaMemoryUsage memory) noexcept {
		_allocInfo.usage = memory;
	}

	VkResult pcdImageAllocate::allocate(rhi::pmvImage& target) const {
		VkImage vk_image;
		VmaAllocation vma_allocation;
		VkResult result = vmaCreateImage(r_allocator, &_imageInfo, &_allocInfo, &vk_image, &vma_allocation, nullptr);
		if (result != VK_SUCCESS)
			return result;

		target.reset();

		target.m_image = vk_image;
		target.m_allocation = vma_allocation;

		target.format = _imageInfo.format;
		target.extent = _imageInfo.extent;

		target.mip_levels = _imageInfo.mipLevels;
		target.array_layers = _imageInfo.arrayLayers;

		target.r_device = r_device;
		target.r_allocator = r_allocator;

		return result;
	}

	void pcdImageAllocate::preset(const VkImageCreateInfo* pCreateInfo) noexcept {
		fetch_basicImageInfo(pCreateInfo);
	}

}