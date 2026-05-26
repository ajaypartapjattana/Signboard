#include "physicalDevice.hh"

#include <bit>

namespace rhi {

	void physicalDevice::enumerateProperties(VkPhysicalDevice physicalDevice) noexcept {
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
		vkGetPhysicalDeviceFeatures(physicalDevice, &features);
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memory);

		{
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

			queueCounts.resize(queueFamilyCount);

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				queueCounts[i] = queueFamilyProperties[i].queueCount;
			}

			uint8_t specializationRating[FAMILY_INDEX_MAX_ENUM]{};
			memset(specializationRating, 0xFF, sizeof(specializationRating));

			uint8_t _QueueFamilyCount_8b = static_cast<uint8_t>(queueFamilyCount);
			for (uint8_t i = 0; i < _QueueFamilyCount_8b; ++i) {
				const VkQueueFamilyProperties& familyProperty = queueFamilyProperties[i];

				uint8_t score = static_cast<uint8_t>(__popcnt(familyProperty.queueFlags));
				for (uint8_t j = 0; j < FAMILY_INDEX_MAX_ENUM; ++j) {
					if ((familyProperty.queueFlags & (1ul << j)) && score < specializationRating[j]) {
						assignedQueueFamilies[j] = i;
						specializationRating[j] = score;
					}
				}
			}
		}

		r_physicalDevice = physicalDevice;

	}

	uint8_t physicalDevice::presentationFamily(VkSurfaceKHR surface) const noexcept {
		VkBool32 presentationSupported = VK_FALSE;

		uint32_t graphicsFamilyIndex = static_cast<uint32_t>(assignedQueueFamilies[FAMILY_INDEX_GRAPHICS]);

		vkGetPhysicalDeviceSurfaceSupportKHR(r_physicalDevice, graphicsFamilyIndex, surface, &presentationSupported);

		if (presentationSupported)
			return assignedQueueFamilies[FAMILY_INDEX_GRAPHICS];

		for (uint8_t i = 1; i < FAMILY_INDEX_MAX_ENUM; ++i) {
			uint32_t FamilyIndex = assignedQueueFamilies[i];
			if (FamilyIndex == graphicsFamilyIndex)
				continue;

			vkGetPhysicalDeviceSurfaceSupportKHR(r_physicalDevice, FamilyIndex, surface, &presentationSupported);
			if (presentationSupported)
				return assignedQueueFamilies[i];
		}

		return 0xff;
	}

}