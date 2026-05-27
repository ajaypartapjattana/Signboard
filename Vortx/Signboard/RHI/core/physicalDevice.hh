#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <array>
#include "Signboard/Core/Containers/span/span.h"

namespace rhi {

	class physicalDevice {
	private:
		VkPhysicalDeviceProperties properties{};
		VkPhysicalDeviceMemoryProperties memory{};
		VkPhysicalDeviceFeatures features{};
		std::vector<VkQueueFamilyProperties> queueFamilies;

		std::vector<VkBool32> queuePresentationSupport;
		VkSurfaceKHR presentationCapabilityCandidate;

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

		sgb::span<const VkQueueFamilyProperties> queueFamilyProperties() const noexcept {
			return queueFamilies;
		}

		void enumerateProperties(VkPhysicalDevice physicalDevice) noexcept;
		sgb::span<const VkBool32> queueFamilyPresentationSupport(VkSurfaceKHR surface) noexcept;

	};

}