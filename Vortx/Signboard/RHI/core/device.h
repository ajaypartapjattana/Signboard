#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "Signboard/RHI/detail/standardQueues.h"

namespace rhi {

	namespace access {
		struct device_pAccess;
	}

	class creInstance;
	class creSurface;

	class pcdQueueSubmission;

	class creDevice {
	public:
		struct createInfo {
			rhi::creSurface* present_surface = nullptr;
			std::vector<const char*> requiredExtensions;
			std::vector<VkBool32 VkPhysicalDeviceFeatures::*> requiredFeatures;
		};

		creDevice(const createInfo& createInfo, const creInstance& instance);

		creDevice(const creDevice&) = delete;
		creDevice& operator=(const creDevice&) = delete;

		creDevice(creDevice&&) noexcept;
		creDevice& operator=(creDevice&&) noexcept;

		~creDevice() noexcept;

		bool active_feature(const VkBool32 VkPhysicalDeviceFeatures::* feature) const noexcept;

	private:
		void build(const createInfo& createInfo, const VkInstance instance);

	private:
		friend class pcdQueueSubmission;
		friend struct access::device_pAccess;

		VkDevice r_device;
		VkPhysicalDevice m_physical;

		standardQueues m_queues;
		standardQueueFamilies m_families;

		std::vector<VkBool32 VkPhysicalDeviceFeatures::*> m_enabledFeatures;

	};

}