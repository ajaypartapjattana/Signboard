#pragma once

namespace rhi::procedure {
	class image_allocator;
	class image_wrapper;
}

#include <vma.h>

namespace rhi::primitive {

	struct image_vkAccess;

	class image {
	public:
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

		image() = default;

		VkImage m_image = VK_NULL_HANDLE;
		VmaAllocation m_allocation = VK_NULL_HANDLE;
		VkImageView m_view = VK_NULL_HANDLE;

		VkDevice m_device = VK_NULL_HANDLE;
		VmaAllocator m_allocator = VK_NULL_HANDLE;

	};

}