#include "context.h"

constexpr const char* validationLayer = "VK_LAYER_KHRONOS_validation";

namespace rndr {

	context::context() {
		{
			VkApplicationInfo application{};
			application.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			application.pNext = nullptr;
			application.pApplicationName = "My Application";
			application.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			application.pEngineName = "Signboard";
			application.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			application.apiVersion = VK_API_VERSION_1_3;

			std::vector<const char*> instanceExtensions;
			std::vector<const char*> instanceLayers;

			uint32_t count = 0;
			const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&count);

			instanceExtensions.reserve(count + 1);
			for (uint32_t i = 0; i < count; ++i) {
				instanceExtensions.emplace_back(glfwExtensions[i]);
			}

#ifdef _VALIDATE
			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			instanceLayers.push_back(validationLayer);
#endif

			VkInstanceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.pApplicationInfo = &application;
			createInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
			createInfo.ppEnabledLayerNames = instanceLayers.data();
			createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
			createInfo.ppEnabledExtensionNames = instanceExtensions.data();

			instance.create(&createInfo);
		}

		std::vector<VkPhysicalDevice> availablePhysicalDevices = instance.enumeratePhysicalDevices();
		physicalDevices.reserve(availablePhysicalDevices.size());
		for (VkPhysicalDevice physicalDevice : availablePhysicalDevices) {
			physicalDevices.emplace_back(physicalDevice);
		}
	}

	float context::createDevice(GLFWwindow* window, uint32_t physicalDeviceIndex) {
		surface.create(instance, window);
		
		rhi::physicalDevice& r_selectedDevice = physicalDevices[physicalDeviceIndex];

		uint8_t presentationFamilyIndex = r_selectedDevice.presentationFamily(surface);
		if (presentationFamilyIndex == UINT8_MAX)
			throw std::runtime_error("INCAPABILITY: physical_device_does_not_support_presentation!");

		float devicePotentialFactor;
		{
			std::vector<float> queueSchedulingPriorities(1, 1.0f);
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

			{
				uint32_t physicalDeviceFamilyCount = r_selectedDevice.maxQueueFamilies();
				std::vector<uint32_t> familyQueueCounts(physicalDeviceFamilyCount, 0);

				for (uint8_t i = 0; i < FAMILY_INDEX_MAX_ENUM; ++i) {
					uint8_t familyIndex = r_selectedDevice.queueFamilyIndex((QueueFamilyType)i);
					++familyQueueCounts[familyIndex];
				}

				uint32_t maxQueueLoad = 1;
				for (uint32_t i = 0; i < physicalDeviceFamilyCount; ++i) {
					uint32_t queueLoad = familyQueueCounts[i];
					if (queueLoad == 0)
						continue;

					maxQueueLoad = std::max(maxQueueLoad, queueLoad);
				}

				devicePotentialFactor = (1.0f / maxQueueLoad);

				queueCreateInfos.reserve(physicalDeviceFamilyCount);

				VkDeviceQueueCreateInfo queueInfo{};
				queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueInfo.pNext = nullptr;
				queueInfo.flags = 0;
				queueInfo.queueCount = 1;

				for (uint32_t i = 0; i < physicalDeviceFamilyCount; ++i) {
					uint32_t requiredQueueCount = familyQueueCounts[i];
					if (requiredQueueCount == 0)
						continue;

					if (requiredQueueCount > r_selectedDevice.queueFamilyCount(i))
						throw std::runtime_error("INCAPABILITY: physical_device_queue_exhausted!");

					queueInfo.queueFamilyIndex = i;
					queueInfo.pQueuePriorities = queueSchedulingPriorities.data();

					queueCreateInfos.emplace_back(queueInfo);
				}

			}

			std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
			VkPhysicalDeviceFeatures enabledFeatures{};
			enabledFeatures.samplerAnisotropy = VK_TRUE;

			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			createInfo.ppEnabledExtensionNames = deviceExtensions.data();
			createInfo.pEnabledFeatures = &enabledFeatures;

			device.create(physicalDevices[physicalDeviceIndex], &createInfo);
		}

		VmaAllocatorCreateInfo createInfo{};
		createInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
		createInfo.physicalDevice = physicalDevices[physicalDeviceIndex];
		createInfo.device = device;
		createInfo.instance = instance;
		createInfo.vulkanApiVersion = VK_API_VERSION_1_3;

		allocator.create(&createInfo);

		return devicePotentialFactor;
	}
		
}
