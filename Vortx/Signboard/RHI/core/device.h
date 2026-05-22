#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "Signboard/RHI/detail/standardQueues.h"

namespace rhi {

	struct _pAccess;

	class creInstance;
	class creSurface;

	class pcdQueueSubmit;

	class creDevice {
	public:
		struct createInfo {
			creSurface* present_surface = nullptr;
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

		VkQueue graphics() const noexcept { return m_queues.graphics; }
		VkQueue compute() const noexcept { return m_queues.compute; }
		VkQueue transfer() const noexcept { return m_queues.transfer; }
		VkQueue preset() const noexcept { return m_queues.present; }

		VkDevice native() const noexcept { return m_device; }

	private:
		void build(const createInfo& createInfo, const VkInstance instance);

	private:
		friend class pcdQueueSubmit;
		friend struct _pAccess;

		VkDevice m_device;
		VkPhysicalDevice m_physical;

		stdQueues m_queues;
		stdQueueFamilies m_families;

		std::vector<VkBool32 VkPhysicalDeviceFeatures::*> m_enabledFeatures;

	};

}