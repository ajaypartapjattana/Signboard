#pragma once

namespace rhi::procedure {
	class device_builder;
}

#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>

namespace rhi::core {

	struct device_vkAccess;

	class device {
	public:
		device(const device&) = delete;
		device& operator=(const device&) = delete;

		device(device&&) noexcept;
		device& operator=(device&&) noexcept;

		~device() noexcept;

		const VkDevice* native_device() const noexcept;

	private:
		friend class rhi::procedure::device_builder;
		friend struct device_vkAccess;

		device() = default;

		VkDevice m_device;
		VkPhysicalDevice m_physical;

		struct queue_entry {
			VkQueue queue;
			uint32_t family;
			VkQueueFlags capabilities;
			bool present_supported;
		};
		std::vector<queue_entry> m_queues;

		VkPhysicalDeviceFeatures m_enabledfeatures;
		VkPhysicalDeviceProperties m_properties;

	};

}