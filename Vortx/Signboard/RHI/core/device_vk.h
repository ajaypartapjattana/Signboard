#pragma once

#include <vulkan/vulkan.h>
#include "device.h"

namespace rhi::core {

	struct device_vkAccess {

		static VkDevice get(const device& d) noexcept {
			return reinterpret_cast<VkDevice>(d.m_device);
		}

		static VkPhysicalDevice get_physicalDevice(const device& d) noexcept {
			return reinterpret_cast<VkPhysicalDevice>(d.m_physical);
		}

		static VkQueue get_queue(const device& d, uint32_t capability) noexcept {
			return reinterpret_cast<VkQueue>(d.m_queues[capability].queue);	//
		}

		static VkAllocationCallbacks* allocator(const device& d) noexcept {
			return reinterpret_cast<VkAllocationCallbacks*>(d.m_allocator);
		}

	};

}