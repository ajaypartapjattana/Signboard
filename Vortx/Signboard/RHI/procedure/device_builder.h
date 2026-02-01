#pragma once

namespace rhi::core {
	class instance;
	class device;
	class surface;
}

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

namespace rhi::procedure {

	class physical_device_selector;

	class device_builder {
	public:
		device_builder(const rhi::core::instance& instance);

		device_builder(const device_builder&) = delete;
		device_builder& operator=(const device_builder&) = delete;

		device_builder& require_graphicsQueue();
		device_builder& require_computeQueue();
		device_builder& require_transferQueue();
		device_builder& require_presentQueue(const rhi::core::surface& s);

		device_builder& enable_samplerAnisotropy();

		rhi::core::device build();


	private:
		friend class physical_device_selector;

		struct phys_candidate {
			VkPhysicalDevice phys = VK_NULL_HANDLE;
			bool suitable = true;

			std::vector<VkQueueFamilyProperties> families;
			std::vector<VkExtensionProperties> extensions;
			VkPhysicalDeviceFeatures features;

			struct assigned_queue {
				uint32_t family;
				VkQueueFlags caps;
				bool can_present = false;
			};

			std::vector<assigned_queue> assigned_queueFamilies;

			VkPhysicalDeviceProperties properties{};
			VkPhysicalDeviceFeatures enabledFeatures{};
		};

		std::vector<phys_candidate> m_candidates;

		std::vector<const char*> m_requiredExtensions;
		VkSurfaceKHR m_surface = VK_NULL_HANDLE;

	};

}