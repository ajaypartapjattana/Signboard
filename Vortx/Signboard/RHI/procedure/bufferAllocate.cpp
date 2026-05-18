#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdBufferAllocate::pcdBufferAllocate(const rhi::creAllocator& allocator, VkBufferCreateInfo* pCreateInfo, VmaAllocationCreateInfo* pAllocInfo) noexcept
		: 
		r_allocator(_pAccess::extract(allocator)),
		memProps(_pAccess::get_memProps(allocator)),
		_bufferInfo(allot_basic(pCreateInfo)),
		_allocInfo(allot_allocInfo(pAllocInfo))
	{

	}

	VkBufferCreateInfo* pcdBufferAllocate::allot_basic(VkBufferCreateInfo* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedBufferInfo = std::make_unique<VkBufferCreateInfo>();

		VkBufferCreateInfo* _info = m_ownedBufferInfo.get();

		VkBufferCreateInfo& info = *_info;
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		return _info;
	}

	VmaAllocationCreateInfo* pcdBufferAllocate::allot_allocInfo(VmaAllocationCreateInfo* pAllocInfo) noexcept {
		if (pAllocInfo)
			return pAllocInfo;

		m_ownedAllocInfo = std::make_unique<VmaAllocationCreateInfo>();

		return m_ownedAllocInfo.get();
	}

	void pcdBufferAllocate::prefer_memory(VmaMemoryUsage memoryType) noexcept {
		_allocInfo->usage = memoryType;
	}

	void pcdBufferAllocate::set_allocationFlags(VmaAllocationCreateFlags allocationFlags) noexcept {
		_allocInfo->flags = allocationFlags;
	}

	void pcdBufferAllocate::usage(VkBufferUsageFlags usage) noexcept {
		_bufferInfo->usage = usage;
	}

	void pcdBufferAllocate::size(VkDeviceSize size) noexcept {
		_bufferInfo->size = size;
	}

	VkResult pcdBufferAllocate::publish(rhi::pmvBuffer& target) const noexcept {
		VmaAllocationInfo allocInfo{};

		VkBuffer _buffer;
		VmaAllocation _allocation;
		VkResult result = vmaCreateBuffer(r_allocator, _bufferInfo, _allocInfo, &_buffer, &_allocation, &allocInfo);
		if (result != VK_SUCCESS)
			return result;

		if (target.r_allocator)
			vmaDestroyBuffer(target.r_allocator, target.m_buffer, target.allocation);

		target.m_buffer = _buffer;
		target.allocation = _allocation;
		target._is_host_coherent = (memProps.memoryTypes[allocInfo.memoryType].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
		target._mpd = allocInfo.pMappedData;
		target.r_allocator = r_allocator;

		return VK_SUCCESS;
	}

	void pcdBufferAllocate::preset(VkBufferCreateInfo* pCreateInfo, VmaAllocationCreateInfo* pAllocInfo) noexcept {
		m_ownedAllocInfo.reset();
		m_ownedBufferInfo.reset();
		_bufferInfo = allot_basic(pCreateInfo);
		_allocInfo = allot_allocInfo(pAllocInfo);
	}

	void pcdBufferAllocate::reset() noexcept {
		m_ownedAllocInfo.reset();
		m_ownedBufferInfo.reset();
		_bufferInfo = allot_basic(nullptr);
		_allocInfo = allot_allocInfo(nullptr);
	}

}