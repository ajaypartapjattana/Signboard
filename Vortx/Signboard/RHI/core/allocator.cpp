#include "allocator.h"

#include <stdexcept>

#include "instance_vk.h"
#include "device_vk.h"

namespace rhi::core {

	allocator::allocator(const instance& instance, const device& device)
		: m_allocator(VK_NULL_HANDLE)
	{
		VkInstance a_instance = rhi::core::instance_vkAccess::get(instance);
		VkDevice a_device = rhi::core::device_vkAccess::get(device);
		VkPhysicalDevice a_phys = rhi::core::device_vkAccess::get_physicalDevice(device);

		m_vkfuncs.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		m_vkfuncs.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		m_vkfuncs.vkGetPhysicalDeviceProperties = &vkGetPhysicalDeviceProperties;
		m_vkfuncs.vkGetPhysicalDeviceMemoryProperties = &vkGetPhysicalDeviceMemoryProperties;
		m_vkfuncs.vkAllocateMemory = &vkAllocateMemory;
		m_vkfuncs.vkFreeMemory = &vkFreeMemory;
		m_vkfuncs.vkMapMemory = &vkMapMemory;
		m_vkfuncs.vkUnmapMemory = &vkUnmapMemory;
		m_vkfuncs.vkFlushMappedMemoryRanges = &vkFlushMappedMemoryRanges;
		m_vkfuncs.vkInvalidateMappedMemoryRanges = &vkInvalidateMappedMemoryRanges;
		m_vkfuncs.vkBindBufferMemory = &vkBindBufferMemory;
		m_vkfuncs.vkBindImageMemory = &vkBindImageMemory;
		m_vkfuncs.vkGetBufferMemoryRequirements = &vkGetBufferMemoryRequirements;
		m_vkfuncs.vkGetImageMemoryRequirements = &vkGetImageMemoryRequirements;
		m_vkfuncs.vkCreateBuffer = &vkCreateBuffer;
		m_vkfuncs.vkDestroyBuffer = &vkDestroyBuffer;
		m_vkfuncs.vkCreateImage = &vkCreateImage;
		m_vkfuncs.vkDestroyImage = &vkDestroyImage;
		m_vkfuncs.vkCmdCopyBuffer = &vkCmdCopyBuffer;

		VmaAllocatorCreateInfo info{};
		info.instance = a_instance;
		info.device = a_device;
		info.physicalDevice = a_phys;
		info.pVulkanFunctions = &m_vkfuncs;
		info.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
		info.vulkanApiVersion = VK_API_VERSION_1_3;

		VkResult result = vmaCreateAllocator(&info, &m_allocator);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: allocator_creation!");

	}

	allocator::allocator(allocator&& other) noexcept {
		m_allocator = other.m_allocator;
		m_vkfuncs = other.m_vkfuncs;

		other.m_allocator = VK_NULL_HANDLE;
	}

	allocator& allocator::operator=(allocator&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_allocator)
			vmaDestroyAllocator(m_allocator);

		m_allocator = other.m_allocator;
		m_vkfuncs = other.m_vkfuncs;

		other.m_allocator = VK_NULL_HANDLE;

		return *this;
	}

	allocator::~allocator() noexcept {
		if (m_allocator)
			vmaDestroyAllocator(m_allocator);
	}

	VmaAllocator allocator::native_allocator() const noexcept {
		return m_allocator;
	}

}