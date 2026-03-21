#pragma once

namespace rhi::procedure {
	class queue_submission;
}

#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>

namespace rhi::core {

	class instance;
	class surface;

	struct device_vkAccess;
	
	class device {
	public:
		struct createInfo {
			rhi::core::surface* present_surface = nullptr;
			std::vector<const char*> requiredExtensions;
			std::vector<VkBool32 VkPhysicalDeviceFeatures::*> requiredFeatures;
		};

		device(const createInfo& createInfo, const instance& instance);

		device(const device&) = delete;
		device& operator=(const device&) = delete;

		device(device&&) noexcept;
		device& operator=(device&&) noexcept;

		~device() noexcept;

		VkDevice native_device() const noexcept;
		bool active_feature(const VkBool32 VkPhysicalDeviceFeatures::* feature) const noexcept;

	private:
		void build(const createInfo& createInfo, const VkInstance instance);

	private:
		friend class rhi::procedure::queue_submission;
		friend struct device_vkAccess;

		VkDevice m_device = VK_NULL_HANDLE;
		VkPhysicalDevice m_physical = VK_NULL_HANDLE;

		struct queues {
			VkQueue graphics = VK_NULL_HANDLE;
			VkQueue compute = VK_NULL_HANDLE;
			VkQueue transfer = VK_NULL_HANDLE;
			VkQueue present = VK_NULL_HANDLE;
		};

		struct queue_families {
			uint32_t graphics = UINT32_MAX;
			uint32_t compute = UINT32_MAX;
			uint32_t transfer = UINT32_MAX;
			uint32_t present = UINT32_MAX;
		};

		queues m_queues;
		queue_families m_families;

		std::vector<VkBool32 VkPhysicalDeviceFeatures::*> m_enabledFeatures;

	};

}