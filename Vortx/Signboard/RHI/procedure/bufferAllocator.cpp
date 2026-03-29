#include "bufferAllocator.h"

#include "Signboard/RHI/primitive/buffer.h"
#include "Signboard/RHI/core/allocator_pAccess.h"

#include <stdexcept>

namespace rhi {

	pcdBufferAllocator::pcdBufferAllocator(const rhi::creAllocator& allocator) noexcept
		: 
		m_allocator(rhi::access::allocator_pAccess::get(allocator)),

		m_bufferInfo(),
		m_allocationInfo()
	{
		m_bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		m_bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;	
	}

	pcdBufferAllocator& pcdBufferAllocator::setMemoryPreference(VmaMemoryUsage memoryType) noexcept {
		m_allocationInfo.usage = memoryType;

		return *this;
	}

	pcdBufferAllocator& pcdBufferAllocator::setMemoryFlags(VmaAllocationCreateFlags allocationFlags) {
		m_allocationInfo.flags = allocationFlags;

		return *this;
	}

	pcdBufferAllocator& pcdBufferAllocator::addUsage(VkBufferUsageFlags usage) noexcept {
		m_bufferInfo.usage |= usage;

		return *this;
	}

	pcdBufferAllocator& pcdBufferAllocator::setBufferSize(VkDeviceSize size) noexcept {
		m_bufferInfo.size = size;
		
		return *this;
	}

	VkResult pcdBufferAllocator::allocateBuffer(rhi::pmvBuffer& tgtBuffer) const noexcept {
		if (m_bufferInfo.size == 0)
			return VK_INCOMPLETE;

		VkResult result = vmaCreateBuffer(m_allocator, &m_bufferInfo, &m_allocationInfo, &tgtBuffer.m_buffer, &tgtBuffer.m_allocation, nullptr);
		if (result != VK_SUCCESS)
			return result;

		tgtBuffer.m_allocator = m_allocator;

		return VK_SUCCESS;
	}

	void* pcdBufferAllocator::mapBuffer(rhi::pmvBuffer& buffer) const {
		void* mapped = nullptr;
		vmaMapMemory(m_allocator, buffer.m_allocation, &mapped);

		return mapped;
	}


}