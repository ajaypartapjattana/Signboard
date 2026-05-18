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

	const VmaVulkanFunctions creAllocator::m_vkfuncs = create_vkfuncs();

	creAllocator::creAllocator(const creInstance& instance, const creDevice& device)
		: 
		m_allocator(VK_NULL_HANDLE)
	{
		VmaAllocatorCreateInfo info{};
		info.instance = _pAccess::extract(instance);
		info.device = _pAccess::extract(device);
		info.physicalDevice = _pAccess::physicalDevice(device);
		info.pVulkanFunctions = &m_vkfuncs;
		info.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
		info.vulkanApiVersion = VK_API_VERSION_1_3;

		VkResult result = vmaCreateAllocator(&info, &m_allocator);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: allocator_creation!");

		vkGetPhysicalDeviceMemoryProperties(_pAccess::physicalDevice(device), &_memProps);

	}

	creAllocator::creAllocator(creAllocator&& other) noexcept 
		:
		m_allocator(other.m_allocator),
		_memProps(other._memProps)
	{
		other.m_allocator = VK_NULL_HANDLE;
	}

	creAllocator& creAllocator::operator=(creAllocator&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_allocator)
			vmaDestroyAllocator(m_allocator);

		m_allocator = other.m_allocator;
		_memProps = other._memProps;

		other.m_allocator = VK_NULL_HANDLE;

		return *this;
	}

	creAllocator::~creAllocator() noexcept {
		if (m_allocator)
			vmaDestroyAllocator(m_allocator);
	}

}