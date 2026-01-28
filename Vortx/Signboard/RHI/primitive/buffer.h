#pragma once

#include <vma.h>

namespace rhi::procedure {
	class buffer_allocator;
}

namespace rhi::primitive {

	struct buffer_vkAccess;

	class buffer {
	public:
		buffer(const buffer&) = delete;
		buffer& operator=(const buffer&) = delete;

		buffer(buffer&&) noexcept;
		buffer& operator=(buffer&&) noexcept;

		~buffer() noexcept;

		VkBuffer native_buffer() const noexcept;

	private:
		friend class rhi::procedure::buffer_allocator;
		friend struct buffer_vkAccess;

		buffer() = default;

		VkBuffer m_buffer = VK_NULL_HANDLE;
		VmaAllocation m_allocation = VK_NULL_HANDLE;

		VmaAllocator m_allocator = VK_NULL_HANDLE;
	};

}