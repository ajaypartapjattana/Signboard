#pragma once

namespace rhi::procedure {
	class image_allocator;
	class image_wrapper;
}

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi::primitive {

	struct image_vkAccess;

	class image {
	public:
		image() noexcept;

		image(const image&) = delete;
		image& operator=(const image&) = delete;

		image(image&&) noexcept;
		image& operator=(image&&) noexcept;

		~image() noexcept;

		const VkImage native_image() const noexcept;
		const VkImageView native_view() const noexcept;


	private:
		friend class rhi::procedure::image_allocator;
		friend class rhi::procedure::image_wrapper;
		friend struct image_vkAccess;

		VkImage m_image = VK_NULL_HANDLE;
		VmaAllocation m_allocation = VK_NULL_HANDLE;
		VkImageView m_view = VK_NULL_HANDLE;

		VkFormat m_format = VK_FORMAT_UNDEFINED;

		VkDevice m_device = VK_NULL_HANDLE;
		VmaAllocator m_allocator = VK_NULL_HANDLE;

	};

}