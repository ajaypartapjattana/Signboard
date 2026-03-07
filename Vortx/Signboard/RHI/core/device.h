#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>

namespace rhi::core {

	class instance;
	class surface;

	struct device_vkAccess;
	
	struct device_CI {
		std::vector<VkQueueFlagBits> requiredQueues;
		rhi::core::surface* present_surface;

		std::vector<const char*> requiredExtensions;
		std::vector<VkBool32 VkPhysicalDeviceFeatures::*> requiredFeatures;
	};

	class device {
	public:
		device(const device_CI& createInfo, const instance& instance);

		device(const device&) = delete;
		device& operator=(const device&) = delete;

		device(device&&) noexcept;
		device& operator=(device&&) noexcept;

		~device() noexcept;

		const VkDevice* native_device() const noexcept;

	private:
		void build(const device_CI& createInfo, const VkInstance instance);

	private:
		friend struct device_vkAccess;

		VkDevice m_device = VK_NULL_HANDLE;
		VkPhysicalDevice m_physical = VK_NULL_HANDLE;

		struct queue_entry {
			VkQueue queue;
			uint32_t family;
			uint32_t index;
			VkQueueFlags capabilities;
			bool present_supported;
		};
		std::vector<queue_entry> m_queues;

		VkPhysicalDeviceFeatures m_enabledfeatures{};
		VkPhysicalDeviceProperties m_properties{};

	};

}