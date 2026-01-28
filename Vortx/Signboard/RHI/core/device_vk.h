#pragma once

#include "device.h"

namespace rhi::core {

	struct device_vkAccess {

		static VkDevice get(const device& d) noexcept {
			return d.m_device;
		}

		static VkPhysicalDevice get_physicalDevice(const device& d) noexcept {
			return d.m_physical;
		}

		static VkQueue get_queue(const device& d, VkQueueFlags capability) noexcept {
			for (const device::queue_entry& e : d.m_queues) {
				if (e.capabilities == capability)
					return e.queue;
			}
		}

		static bool get_featureEnabled(const device& d, const VkBool32 VkPhysicalDeviceFeatures::* feature) {
			return d.m_enabledfeatures.*feature == VK_TRUE;
		}

		static const VkPhysicalDeviceProperties& get_properties(const device& d) {
			return d.m_properties;
		}

	};

}