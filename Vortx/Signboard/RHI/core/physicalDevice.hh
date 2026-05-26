#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <array>

enum QueueFamilyType : uint8_t {
	FAMILY_INDEX_GRAPHICS,
	FAMILY_INDEX_COMPUTE,
	FAMILY_INDEX_TRANSFER,

	FAMILY_INDEX_MAX_ENUM
};

namespace rhi {

	class physicalDevice {
	private:
		VkPhysicalDeviceProperties properties{};
		VkPhysicalDeviceMemoryProperties memory{};
		VkPhysicalDeviceFeatures features{};
		std::vector<uint32_t> queueCounts;

		std::array<uint8_t, FAMILY_INDEX_MAX_ENUM> assignedQueueFamilies{};

		VkPhysicalDevice r_physicalDevice = VK_NULL_HANDLE;

	public:
		physicalDevice() = default;
		physicalDevice(VkPhysicalDevice physicalDevice) noexcept {
			enumerateProperties(physicalDevice);
		}
		physicalDevice(const physicalDevice&) = default;
		physicalDevice(physicalDevice&&) = default;

		physicalDevice& operator=(const physicalDevice&) = default;
		physicalDevice& operator=(physicalDevice&&) = default;

		~physicalDevice() = default;

		operator VkPhysicalDevice() const noexcept {
			return r_physicalDevice;
		}

		const char* deviceName() const noexcept {
			return properties.deviceName;
		}

		uint8_t maxQueueFamilies() const noexcept {
			return queueCounts.size();
		}

		uint8_t queueFamilyIndex(QueueFamilyType type) const noexcept {
			return assignedQueueFamilies[type];
		}

		uint32_t queueFamilyCount(uint32_t familyIndex) const noexcept {
			return queueCounts[familyIndex];
		}

		void enumerateProperties(VkPhysicalDevice physicalDevice) noexcept;
		uint8_t presentationFamily(VkSurfaceKHR surface) const noexcept;

	};

}