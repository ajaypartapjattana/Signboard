#pragma once

namespace rhi::core {
	class device;
	class allocator;
}

namespace rhi::primitive {
	class image;
}

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi::procedure {

	class image_allocator {
	public:
		image_allocator(const rhi::core::device& deivce, const rhi::core::allocator& allocator);

		image_allocator(const image_allocator&) = delete;
		image_allocator& operator=(const image_allocator&) = delete;

		image_allocator& set_usage(VkImageUsageFlags usage) noexcept;
		image_allocator& set_format(VkFormat format) noexcept;
		image_allocator& set_aspect(VkImageAspectFlags aspect) noexcept;

		image_allocator& set_extent(VkExtent2D extent) noexcept;
		image_allocator& set_extent(VkExtent3D extent) noexcept;

		VkResult allocate(rhi::primitive::image& targetImage) const;

	private:

		void reset_allocatorState();

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VmaAllocator m_allocator = VK_NULL_HANDLE;

		VkImageUsageFlags final_usage{};
		VkFormat final_format{};
		VkExtent3D final_extent{};
		VkImageAspectFlags final_aspect{};

	};

}