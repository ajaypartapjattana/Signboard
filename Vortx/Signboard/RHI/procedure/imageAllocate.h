#pragma once

#include <memory>

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi {

	class creDevice;
	class creAllocator;

	class pmvImage;

	class pcdImageAllocate {
	public:
		pcdImageAllocate(const creAllocator& allocator, VkImageCreateInfo* pCreateInfo = nullptr, VmaAllocationCreateInfo* pAllocInfo = nullptr) noexcept;

		pcdImageAllocate(const pcdImageAllocate&) = delete;
		pcdImageAllocate& operator=(const pcdImageAllocate&) = delete;

		void usage(VkImageUsageFlags usage) noexcept;
		void format(VkFormat format) noexcept;
		void extent(VkExtent3D extent) noexcept;

		void extend(uint32_t mipLevels, uint32_t arrayLayers) noexcept;
		void sampling(VkSampleCountFlagBits sampleCount) noexcept;
		void sharing(VkSharingMode mode) noexcept;

		void memory(VmaMemoryUsage memory) noexcept;

		VkResult publish(pmvImage& targetImage) const noexcept;

		void preset(VkImageCreateInfo* pCreateInfo, VmaAllocationCreateInfo* pAllocInfo) noexcept;
		void reset() noexcept;

	private:
		VkImageCreateInfo* allot_imageInfo(VkImageCreateInfo* pCreateInfo) noexcept;
		VmaAllocationCreateInfo* allot_allocInfo(VmaAllocationCreateInfo* pAllocInfo) noexcept;

	private:
		const VmaAllocator r_allocator;

		std::unique_ptr<VkImageCreateInfo> m_ownedImageInfo;
		VkImageCreateInfo* pImageInfo;

		std::unique_ptr<VmaAllocationCreateInfo> m_ownedAllocInfo;
		VmaAllocationCreateInfo* pAllocInfo;

	};

}