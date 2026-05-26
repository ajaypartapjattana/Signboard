#include <stdexcept>

#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	static VmaVulkanFunctions create_vkfuncs() {
		VmaVulkanFunctions f{};
		f.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		f.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		f.vkGetPhysicalDeviceProperties = &vkGetPhysicalDeviceProperties;
		f.vkGetPhysicalDeviceMemoryProperties = &vkGetPhysicalDeviceMemoryProperties;
		f.vkAllocateMemory = &vkAllocateMemory;
		f.vkFreeMemory = &vkFreeMemory;
		f.vkMapMemory = &vkMapMemory;
		f.vkUnmapMemory = &vkUnmapMemory;
		f.vkFlushMappedMemoryRanges = &vkFlushMappedMemoryRanges;
		f.vkInvalidateMappedMemoryRanges = &vkInvalidateMappedMemoryRanges;
		f.vkBindBufferMemory = &vkBindBufferMemory;
		f.vkBindImageMemory = &vkBindImageMemory;
		f.vkGetBufferMemoryRequirements = &vkGetBufferMemoryRequirements;
		f.vkGetImageMemoryRequirements = &vkGetImageMemoryRequirements;
		f.vkCreateBuffer = &vkCreateBuffer;
		f.vkDestroyBuffer = &vkDestroyBuffer;
		f.vkCreateImage = &vkCreateImage;
		f.vkDestroyImage = &vkDestroyImage;
		f.vkCmdCopyBuffer = &vkCmdCopyBuffer;
		return f;
	}

	const VmaVulkanFunctions allocator::m_vkfuncs = create_vkfuncs();

	allocator::allocator(const instance& instance, const device& device)
		: 
		m_allocator(VK_NULL_HANDLE)
	{
		

		vkGetPhysicalDeviceMemoryProperties(_pAccess::physicalDevice(device), &_memProps);

	}

	allocator::allocator(allocator&& other) noexcept 
		:
		m_allocator(other.m_allocator),
		_memProps(other._memProps)
	{
		other.m_allocator = VK_NULL_HANDLE;
	}

	allocator& allocator::operator=(allocator&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_allocator)
			vmaDestroyAllocator(m_allocator);

		m_allocator = other.m_allocator;
		_memProps = other._memProps;

		other.m_allocator = VK_NULL_HANDLE;

		return *this;
	}

	allocator::~allocator() noexcept {
		
	}

	VkResult allocator::create(VmaAllocatorCreateInfo* pCreateInfo) noexcept {
		VmaAllocator allocator;
		VkResult result = vmaCreateAllocator(pCreateInfo, &allocator);
		if (result != VK_SUCCESS)
			return result;

		reset();

		m_allocator = allocator;
				
	}

	void allocator::reset() noexcept {
		if (m_allocator)
			vmaDestroyAllocator(m_allocator);

		m_allocator = VK_NULL_HANDLE;
	}

}