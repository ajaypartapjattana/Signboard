#include "bufferAllocator.h"

#include "Signboard/RHI/primitive/buffer.h"
#include "Signboard/RHI/core/allocator_pAccess.h"

#include <stdexcept>

namespace rhi {

	pcdBufferAllocator::pcdBufferAllocator(const rhi::creAllocator& allocator) noexcept
		: 
		_allctr(rhi::access::allocator_pAccess::get(allocator)),
		_memProps(rhi::access::allocator_pAccess::get_memProps(allocator)),

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

	pcdBufferAllocator& pcdBufferAllocator::setMemoryFlags(VmaAllocationCreateFlags allocationFlags) noexcept {
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

	VkResult pcdBufferAllocator::allocateBuffer(rhi::pmvBuffer& tgt) const noexcept {
		if (m_bufferInfo.size == 0)
			return VK_INCOMPLETE;

		VmaAllocationInfo allocInfo{};

		VkResult result = vmaCreateBuffer(_allctr, &m_bufferInfo, &m_allocationInfo, &tgt.m_buffer, &tgt.allocation, &allocInfo);

		if (result != VK_SUCCESS)
			return result;

		tgt._allctr = _allctr;

		tgt._is_host_coherent = (_memProps.memoryTypes[allocInfo.memoryType].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
		tgt._mpd = allocInfo.pMappedData;

		return VK_SUCCESS;
	}

}