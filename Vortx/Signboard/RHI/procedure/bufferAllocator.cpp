#include "bufferAllocator.h"

#include "Signboard/RHI/primitive/buffer.h"
#include "Signboard/RHI/core/allocator_pAccess.h"

#include <stdexcept>

namespace rhi {

	pcdBufferAllocator::pcdBufferAllocator(const rhi::creAllocator& allocator) noexcept
		: 
		m_allocator(rhi::access::allocator_pAccess::get(allocator)),
		requestedMemory(VMA_MEMORY_USAGE_AUTO),
		requestedUsage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT),
		requestedSize(0)
	{

	}

	pcdBufferAllocator& pcdBufferAllocator::setMemoryPreference(VmaMemoryUsage usage) noexcept {
		requestedMemory = usage;
	}

	pcdBufferAllocator& pcdBufferAllocator::addUsage(VkBufferUsageFlags usage) noexcept {
		requestedUsage |= usage;
	}

	pcdBufferAllocator& pcdBufferAllocator::setBufferSize(VkDeviceSize size) noexcept {
		requestedSize = size;
		return *this;
	}

	VkResult pcdBufferAllocator::allocateBuffer(rhi::pmvBuffer& tgtBuffer) const noexcept {
		if (requestedSize == 0)
			return VK_INCOMPLETE;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = requestedSize;
		bufferInfo.usage = requestedUsage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = requestedMemory;

		VkResult result = vmaCreateBuffer(m_allocator, &bufferInfo, &allocInfo, &tgtBuffer.m_buffer, &tgtBuffer.m_allocation, nullptr);
		if (result != VK_SUCCESS)
			return result;

		tgtBuffer.m_allocator = m_allocator;

		return VK_SUCCESS;
	}

}