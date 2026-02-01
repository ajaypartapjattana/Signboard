#include "allocator_creator.h"

#include "Signboard/RHI/core/allocator.h"
#include <Signboard/RHI/core/instance_vk.h>
#include <Signboard/RHI/core/device_vk.h>

#include <stdexcept>

namespace rhi::procedure {

	allocator_creator::allocator_creator(const rhi::core::instance& instance, const rhi::core::device& device) 
		: m_instance(rhi::core::instance_vkAccess::get(instance)),
		m_device(rhi::core::device_vkAccess::get(device)),
		m_phys(rhi::core::device_vkAccess::get_physicalDevice(device))
	{

	}

	rhi::core::allocator allocator_creator::create() const {
		rhi::core::allocator l_mallocator;

		l_mallocator.m_vkfuncs.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		l_mallocator.m_vkfuncs.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		l_mallocator.m_vkfuncs.vkGetPhysicalDeviceProperties = &vkGetPhysicalDeviceProperties;
		l_mallocator.m_vkfuncs.vkGetPhysicalDeviceMemoryProperties = &vkGetPhysicalDeviceMemoryProperties;
		l_mallocator.m_vkfuncs.vkAllocateMemory = &vkAllocateMemory;
		l_mallocator.m_vkfuncs.vkFreeMemory = &vkFreeMemory;
		l_mallocator.m_vkfuncs.vkMapMemory = &vkMapMemory;
		l_mallocator.m_vkfuncs.vkUnmapMemory = &vkUnmapMemory;
		l_mallocator.m_vkfuncs.vkFlushMappedMemoryRanges = &vkFlushMappedMemoryRanges;
		l_mallocator.m_vkfuncs.vkInvalidateMappedMemoryRanges = &vkInvalidateMappedMemoryRanges;
		l_mallocator.m_vkfuncs.vkBindBufferMemory = &vkBindBufferMemory;
		l_mallocator.m_vkfuncs.vkBindImageMemory = &vkBindImageMemory;
		l_mallocator.m_vkfuncs.vkGetBufferMemoryRequirements = &vkGetBufferMemoryRequirements;
		l_mallocator.m_vkfuncs.vkGetImageMemoryRequirements = &vkGetImageMemoryRequirements;
		l_mallocator.m_vkfuncs.vkCreateBuffer = &vkCreateBuffer;
		l_mallocator.m_vkfuncs.vkDestroyBuffer = &vkDestroyBuffer;
		l_mallocator.m_vkfuncs.vkCreateImage = &vkCreateImage;
		l_mallocator.m_vkfuncs.vkDestroyImage = &vkDestroyImage;
		l_mallocator.m_vkfuncs.vkCmdCopyBuffer = &vkCmdCopyBuffer;

		VmaAllocatorCreateInfo info{};
		info.instance = m_instance;
		info.device = m_device;
		info.physicalDevice = m_phys;
		info.pVulkanFunctions = &l_mallocator.m_vkfuncs;
		info.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
		info.vulkanApiVersion = VK_API_VERSION_1_3;

		VmaAllocator vma_allocator = VK_NULL_HANDLE;
		if (vmaCreateAllocator(&info, &vma_allocator) != VK_SUCCESS)
			throw std::runtime_error("FAILED: vma_creation!");

		l_mallocator.m_allocator = vma_allocator;

		return l_mallocator;
	}

}
