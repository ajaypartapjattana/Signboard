#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi {

	class creDevice;
	class creAllocator;

	class pmvImage;

	class pcdImageAllocator {
	public:
		pcdImageAllocator(const rhi::creDevice& deivce, const rhi::creAllocator& allocator);

		pcdImageAllocator(const pcdImageAllocator&) = delete;
		pcdImageAllocator& operator=(const pcdImageAllocator&) = delete;

		pcdImageAllocator& set_usage(VkImageUsageFlags usage) noexcept;
		pcdImageAllocator& set_format(VkFormat format) noexcept;
		pcdImageAllocator& set_aspect(VkImageAspectFlags aspect) noexcept;

		pcdImageAllocator& set_extent(VkExtent2D extent) noexcept;
		pcdImageAllocator& set_extent(VkExtent3D extent) noexcept;

		VkResult allocate(rhi::pmvImage& targetImage) const;

	private:
		VkDevice m_device;
		VmaAllocator m_allocator;

		VkImageUsageFlags final_usage{};
		VkFormat final_format{};
		VkExtent3D final_extent{};
		VkImageAspectFlags final_aspect{};

	};

}