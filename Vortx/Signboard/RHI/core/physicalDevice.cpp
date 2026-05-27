#include "physicalDevice.hh"

#include <bit>

namespace rhi {

	void physicalDevice::enumerateProperties(VkPhysicalDevice physicalDevice) noexcept {
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
		vkGetPhysicalDeviceFeatures(physicalDevice, &features);
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memory);

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		queueFamilies.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		queuePresentationSupport.resize(queueFamilyCount, VK_FALSE);

		r_physicalDevice = physicalDevice;
	}

	sgb::span<const VkBool32> physicalDevice::queueFamilyPresentationSupport(VkSurfaceKHR surface) noexcept {
		if (surface == presentationCapabilityCandidate)
			return queuePresentationSupport;

		uint32_t queueFamilyCount = static_cast<uint32_t>(queueFamilies.size());
		for (uint32_t i = 0; i < queueFamilyCount; ++i) {
			vkGetPhysicalDeviceSurfaceSupportKHR(r_physicalDevice, i, surface, &queuePresentationSupport[i]);
		}

		presentationCapabilityCandidate = surface;

		return queuePresentationSupport;
	}

}