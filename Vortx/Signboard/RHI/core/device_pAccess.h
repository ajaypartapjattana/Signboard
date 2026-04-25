#pragma once

#include "device.h"

namespace rhi::access {

	struct device_pAccess {

		static VkDevice extract(const rhi::creDevice& d) noexcept {
			return d._dvc;
		}

		static VkPhysicalDevice get_physicalDevice(const rhi::creDevice& d) noexcept {
			return d.m_physical;
		}

		static const standardQueues& get_queues(const rhi::creDevice& d) noexcept {
			return d.m_queues;
		}

		static const standardQueueFamilies& get_queueFamilies(const rhi::creDevice& d) noexcept {
			return d.m_families;
		}

	};

}