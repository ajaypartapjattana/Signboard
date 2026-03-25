#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi::access {
	struct buffer_vkAccess;
}

namespace rhi {

	class pcdBufferAllocator;

	class pmvBuffer {
	public:
		pmvBuffer() noexcept;

		pmvBuffer(const pmvBuffer&) = delete;
		pmvBuffer& operator=(const pmvBuffer&) = delete;

		pmvBuffer(pmvBuffer&&) noexcept;
		pmvBuffer& operator=(pmvBuffer&&) noexcept;

		~pmvBuffer() noexcept;

	private:
		friend class pcdBufferAllocator;
		friend struct rhi::access::buffer_vkAccess;

		VkBuffer m_buffer = VK_NULL_HANDLE;
		VmaAllocation m_allocation = VK_NULL_HANDLE;

		VmaAllocator m_allocator = VK_NULL_HANDLE;

	};

}