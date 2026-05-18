#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdImageAllocate::pcdImageAllocate(const creAllocator& allocator,VkImageCreateInfo* pCreateInfo, VmaAllocationCreateInfo* pAllocInfo) noexcept
		: 
		r_allocator(_pAccess::extract(allocator)),

		pImageInfo(allot_imageInfo(pCreateInfo)),
		pAllocInfo(allot_allocInfo(pAllocInfo))
	{

	}

	VkImageCreateInfo* pcdImageAllocate::allot_imageInfo(VkImageCreateInfo* pInfo) noexcept {
		if (pInfo)
			return pInfo;

		m_ownedImageInfo = std::make_unique<VkImageCreateInfo>();

		VkImageCreateInfo* _info = m_ownedImageInfo.get();
		_info->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

		_info->mipLevels = 1;
		_info->arrayLayers = 1;
		_info->samples = VK_SAMPLE_COUNT_1_BIT;
		
		return _info;
	}

	VmaAllocationCreateInfo* pcdImageAllocate::allot_allocInfo(VmaAllocationCreateInfo* pInfo) noexcept {
		if (pInfo)
			return pInfo;

		m_ownedAllocInfo = std::make_unique<VmaAllocationCreateInfo>();

		return m_ownedAllocInfo.get();
	}

	void pcdImageAllocate::usage(VkImageUsageFlags usage) noexcept {
		pImageInfo->usage = usage;
	}

	void pcdImageAllocate::format(VkFormat format) noexcept {
		pImageInfo->format = format;
	}

	void pcdImageAllocate::extent(VkExtent3D extent) noexcept {
		pImageInfo->extent = extent;
	}

	void pcdImageAllocate::extend(uint32_t mipLevels, uint32_t arrayLayers) noexcept {
		pImageInfo->mipLevels = mipLevels;
		pImageInfo->arrayLayers = arrayLayers;
	}

	void pcdImageAllocate::sampling(VkSampleCountFlagBits sampleCount) noexcept {
		pImageInfo->samples = sampleCount;
	}

	void pcdImageAllocate::sharing(VkSharingMode mode) noexcept {
		pImageInfo->sharingMode = mode;
	}

	void pcdImageAllocate::memory(VmaMemoryUsage memory) noexcept {
		pAllocInfo->usage = memory;
	}

	VkResult pcdImageAllocate::publish(pmvImage& target) const noexcept {
		VkImage _image;
		VmaAllocation _allocation;
		VkResult result = vmaCreateImage(r_allocator, pImageInfo, pAllocInfo, &_image, &_allocation, nullptr);
		if (result != VK_SUCCESS)
			return result;

		target.reset();

		target.m_image = _image;
		target.m_allocation = _allocation;

		target.format = pImageInfo->format;
		target.extent = pImageInfo->extent;

		target.mip_levels = pImageInfo->mipLevels;
		target.array_layers = pImageInfo->arrayLayers;

		target.r_allocator = r_allocator;

		return result;
	}

	void pcdImageAllocate::preset(VkImageCreateInfo* pCreateInfo, VmaAllocationCreateInfo* pInfo) noexcept {
		m_ownedAllocInfo.reset();
		m_ownedImageInfo.reset();
		pImageInfo = allot_imageInfo(pCreateInfo);
		pAllocInfo = allot_allocInfo(pInfo);
	}

	void pcdImageAllocate::reset() noexcept {
		m_ownedAllocInfo.reset();
		m_ownedImageInfo.reset();
		pImageInfo = allot_imageInfo(nullptr);
		pAllocInfo = allot_allocInfo(nullptr);
	}

}