#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi {

	class creAllocator;

	class pmvBuffer;

	class pcdBufferAllocator {
	public:
		pcdBufferAllocator(const rhi::creAllocator& allocator) noexcept;

		pcdBufferAllocator(const pcdBufferAllocator&) = delete;
		pcdBufferAllocator& operator=(const pcdBufferAllocator&) = delete;

		pcdBufferAllocator& addUsage(VkBufferUsageFlags usage) noexcept;
		pcdBufferAllocator& setMemoryPreference(VmaMemoryUsage memory) noexcept;
		pcdBufferAllocator& setMemoryFlags(VmaAllocationCreateFlags allocationFlags) noexcept;
		pcdBufferAllocator& setBufferSize(VkDeviceSize size) noexcept;

		VkResult allocateBuffer(rhi::pmvBuffer& targetBuffer) const noexcept;
		void* mapBuffer(rhi::pmvBuffer& buffer) const;

	private:
		VmaAllocator m_allocator;

		VkBufferCreateInfo m_bufferInfo;
		VmaAllocationCreateInfo m_allocationInfo;

	};

}