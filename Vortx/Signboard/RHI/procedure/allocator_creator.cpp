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
		VmaAllocatorCreateInfo info{};
		info.instance = m_instance;
		info.device = m_device;
		info.physicalDevice = m_phys;
		info.vulkanApiVersion = VK_API_VERSION_1_3;

		VmaAllocator vma_allocator = VK_NULL_HANDLE;
		if (vmaCreateAllocator(&info, &vma_allocator) != VK_SUCCESS)
			throw std::runtime_error("FAILED: vma_creation!");

		rhi::core::allocator l_mallocator;
		l_mallocator.m_allocator = vma_allocator;

		return l_mallocator;
	}

}
