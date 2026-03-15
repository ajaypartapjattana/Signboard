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

		static const device::queues& get_queues(const device& d) noexcept {
			return d.m_queues;
		}

		static const device::queue_families& get_queueFamilies(const device& d) noexcept {
			return d.m_families;
		}

	};

}