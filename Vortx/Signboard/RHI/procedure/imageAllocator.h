#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi {

	class creDevice;
	class creAllocator;

	class pmvImage;

	class pcdImageAllocate {
	public:
		pcdImageAllocate(const rhi::creDevice& deivce, const rhi::creAllocator& allocator, const VkImageCreateInfo* pCreateInfo = nullptr);

		pcdImageAllocate(const pcdImageAllocate&) = delete;
		pcdImageAllocate& operator=(const pcdImageAllocate&) = delete;

		void image_usage(VkImageUsageFlags usage) noexcept;
		void image_format(VkFormat format) noexcept;
		void image_extent(VkExtent3D extent) noexcept;

		void allocation_usage(VmaMemoryUsage memory) noexcept;

		VkResult allocate(rhi::pmvImage& targetImage) const;

		void preset(const VkImageCreateInfo* pCreateInfo) noexcept;

	private:
		VkImageCreateInfo fetch_basicImageInfo(const VkImageCreateInfo* pCreateInfo) const noexcept;

	private:
		VkDevice r_device;
		VmaAllocator r_allocator;

		VkImageCreateInfo _imageInfo;
		VmaAllocationCreateInfo _allocInfo;

	};

}