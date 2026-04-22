#include "bufferAllocator.h"

#include "Signboard/RHI/primitive/buffer.h"
#include "Signboard/RHI/core/allocator_pAccess.h"

namespace rhi {

	pcdBufferAllocate::pcdBufferAllocate(const rhi::creAllocator& allocator, VkBufferCreateInfo* pCreateInfo) noexcept
		: 
		r_allocator(rhi::access::allocator_pAccess::get(allocator)),
		memProps(rhi::access::allocator_pAccess::get_memProps(allocator)),
		_bufferInfo(fetch_basic(pCreateInfo)),
		_allocInfo()
	{

	}

	VkBufferCreateInfo pcdBufferAllocate::fetch_basic(VkBufferCreateInfo* pCreateInfo) const noexcept {
		if (pCreateInfo)
			return *pCreateInfo;

		VkBufferCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		return info;
	}


	pcdBufferAllocate& pcdBufferAllocate::prefer_memory(VmaMemoryUsage memoryType) noexcept {
		_allocInfo.usage = memoryType;

		return *this;
	}

	pcdBufferAllocate& pcdBufferAllocate::set_allocationFlags(VmaAllocationCreateFlags allocationFlags) noexcept {
		_allocInfo.flags = allocationFlags;

		return *this;
	}

	pcdBufferAllocate& pcdBufferAllocate::add_usage(VkBufferUsageFlags usage) noexcept {
		_bufferInfo.usage |= usage;

		return *this;
	}

	pcdBufferAllocate& pcdBufferAllocate::size(VkDeviceSize size) noexcept {
		_bufferInfo.size = size;
		
		return *this;
	}

	VkResult pcdBufferAllocate::publish(rhi::pmvBuffer& target) const noexcept {
		if (_bufferInfo.size == 0)
			return VK_INCOMPLETE;

		VmaAllocationInfo allocInfo{};

		VkResult result = vmaCreateBuffer(r_allocator, &_bufferInfo, &_allocInfo, &target.m_buffer, &target.allocation, &allocInfo);
		if (result != VK_SUCCESS)
			return result;

		target.r_allocator = r_allocator;
		target._is_host_coherent = (memProps.memoryTypes[allocInfo.memoryType].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
		target._mpd = allocInfo.pMappedData;

		return VK_SUCCESS;
	}

	void pcdBufferAllocate::preset(VkBufferCreateInfo* pCreateInfo, VmaAllocationCreateInfo* pAllocInfo) noexcept {
		_bufferInfo = fetch_basic(pCreateInfo);

		if (pAllocInfo) {
			_allocInfo = *pAllocInfo;
			return;
		}

		_allocInfo = VmaAllocationCreateInfo{};
	}

	void pcdBufferAllocate::reset() noexcept {
		_bufferInfo = fetch_basic(nullptr);
		_allocInfo = VmaAllocationCreateInfo{};
	}

}