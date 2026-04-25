#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi {

	class creDevice;
	class creAllocator;

	class pmvImage;

	class pcdImageAllocate {
	public:
		pcdImageAllocate(const rhi::creDevice& deivce, const rhi::creAllocator& allocator);

		pcdImageAllocate(const pcdImageAllocate&) = delete;
		pcdImageAllocate& operator=(const pcdImageAllocate&) = delete;

		pcdImageAllocate& set_usage(VkImageUsageFlags usage) noexcept;
		pcdImageAllocate& set_format(VkFormat format) noexcept;
		pcdImageAllocate& set_aspect(VkImageAspectFlags aspect) noexcept;

		pcdImageAllocate& set_extent(VkExtent2D extent) noexcept;
		pcdImageAllocate& set_extent(VkExtent3D extent) noexcept;

		VkResult allocate(rhi::pmvImage& targetImage) const;

	private:
		VkDevice _dvc;
		VmaAllocator _allctr;

		VkImageUsageFlags final_usage{};
		VkFormat final_format{};
		VkExtent3D final_extent{};
		VkImageAspectFlags final_aspect{};

	};

}