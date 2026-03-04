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

	VkResult allocator_creator::create(rhi::core::allocator& target_allocator) const {
		target_allocator.m_vkfuncs.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		target_allocator.m_vkfuncs.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		target_allocator.m_vkfuncs.vkGetPhysicalDeviceProperties = &vkGetPhysicalDeviceProperties;
		target_allocator.m_vkfuncs.vkGetPhysicalDeviceMemoryProperties = &vkGetPhysicalDeviceMemoryProperties;
		target_allocator.m_vkfuncs.vkAllocateMemory = &vkAllocateMemory;
		target_allocator.m_vkfuncs.vkFreeMemory = &vkFreeMemory;
		target_allocator.m_vkfuncs.vkMapMemory = &vkMapMemory;
		target_allocator.m_vkfuncs.vkUnmapMemory = &vkUnmapMemory;
		target_allocator.m_vkfuncs.vkFlushMappedMemoryRanges = &vkFlushMappedMemoryRanges;
		target_allocator.m_vkfuncs.vkInvalidateMappedMemoryRanges = &vkInvalidateMappedMemoryRanges;
		target_allocator.m_vkfuncs.vkBindBufferMemory = &vkBindBufferMemory;
		target_allocator.m_vkfuncs.vkBindImageMemory = &vkBindImageMemory;
		target_allocator.m_vkfuncs.vkGetBufferMemoryRequirements = &vkGetBufferMemoryRequirements;
		target_allocator.m_vkfuncs.vkGetImageMemoryRequirements = &vkGetImageMemoryRequirements;
		target_allocator.m_vkfuncs.vkCreateBuffer = &vkCreateBuffer;
		target_allocator.m_vkfuncs.vkDestroyBuffer = &vkDestroyBuffer;
		target_allocator.m_vkfuncs.vkCreateImage = &vkCreateImage;
		target_allocator.m_vkfuncs.vkDestroyImage = &vkDestroyImage;
		target_allocator.m_vkfuncs.vkCmdCopyBuffer = &vkCmdCopyBuffer;

		VmaAllocatorCreateInfo info{};
		info.instance = m_instance;
		info.device = m_device;
		info.physicalDevice = m_phys;
		info.pVulkanFunctions = &target_allocator.m_vkfuncs;
		info.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
		info.vulkanApiVersion = VK_API_VERSION_1_3;

		VmaAllocator vma_allocator = VK_NULL_HANDLE;
		VkResult result = vmaCreateAllocator(&info, &vma_allocator);
		if (result != VK_SUCCESS)
			return result;

		target_allocator.m_allocator = vma_allocator;

		return result;
	}

}
