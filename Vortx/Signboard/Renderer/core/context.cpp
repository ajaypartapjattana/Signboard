#include "context.h"

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
			instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
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

	void context::createDevice(GLFWwindow* window, uint32_t physicalDeviceIndex) {
		rhi::physicalDevice& r_selectedDevice = physicalDevices[physicalDeviceIndex];

		{
			rhi::surface l_surface{ instance, window };

			sgb::span<const VkBool32> presentationSupport = r_selectedDevice.queueFamilyPresentationSupport(l_surface);
			uint32_t familyCount = static_cast<uint32_t>(presentationSupport.size());

			{
				bool offscreenDevice = true;
				for (uint32_t i = 0; i < familyCount; ++i) {
					if (!presentationSupport[i])
						continue;

					offscreenDevice = false;
					break;
				}

				if (offscreenDevice)
					throw std::runtime_error("INCAPABILITY: physical_device_does_not_support_presentation!");
			}

			std::array<uint32_t, FAMILY_INDEX_MAX_ENUM> l_assignedFamilies{};
			l_assignedFamilies.fill(UINT32_MAX);

			{
				sgb::span<const VkQueueFamilyProperties> queueFamilyProperties = r_selectedDevice.queueFamilyProperties();

				std::array<uint32_t, FAMILY_INDEX_MAX_ENUM> familyCapabilities{};
				familyCapabilities.fill(UINT32_MAX);

				constexpr VkQueueFlags familyFlags[] = { VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT };

				for (uint32_t i = 0; i < familyCount; ++i) {
					const VkQueueFamilyProperties& familyProperty = queueFamilyProperties[i];

					uint32_t relevantFlags = 0;

					for (uint32_t j = 0; j < FAMILY_INDEX_MAX_ENUM; ++j) {
						uint32_t flag = familyFlags[j];
						relevantFlags |= (familyProperty.queueFlags & flag) ? flag : 0;
					}

					uint32_t queueFamilyCapabilityCount = __popcnt(relevantFlags);

					for (uint32_t j = 0; j < FAMILY_INDEX_MAX_ENUM; ++j) {
						if ((familyProperty.queueFlags & familyFlags[j]) && queueFamilyCapabilityCount < familyCapabilities[j]) {
							l_assignedFamilies[j] = i;
							familyCapabilities[j] = queueFamilyCapabilityCount;
						}
					}
				}

				for (uint32_t i = 0; i < FAMILY_INDEX_MAX_ENUM; ++i) {
					if (l_assignedFamilies[i] == UINT32_MAX)
						throw std::runtime_error("INCAPABILITY: physical_device_has_insufficient_traits!");
				}
			}

			float queueSchedulingPriorities = 1.0f;
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

			uint32_t l_assignedPresentFamily = UINT32_MAX;

			{
				std::vector<uint8_t> requiredFamilies(familyCount, false);

				for (uint32_t i = 0; i < FAMILY_INDEX_MAX_ENUM; ++i) {
					uint32_t familyIndex = l_assignedFamilies[(QueueFamilyType)i];
					requiredFamilies[familyIndex] = true;
				}

				if (presentationSupport[l_assignedFamilies[FAMILY_INDEX_GRAPHICS]]) {
					l_assignedPresentFamily = l_assignedFamilies[FAMILY_INDEX_GRAPHICS];
				} else {
					for (uint32_t i = 0; i < familyCount; ++i) {
						if (!presentationSupport[i])
							continue;

						requiredFamilies[i] = true;
						l_assignedPresentFamily = i;
						break;
					}
				}

				VkDeviceQueueCreateInfo queueInfo{};
				queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueInfo.pNext = nullptr;
				queueInfo.flags = 0;
				queueInfo.queueCount = 1;

				queueCreateInfos.reserve(FAMILY_INDEX_MAX_ENUM);

				for (uint32_t i = 0; i < familyCount; ++i) {
					if (requiredFamilies[i] == false)
						continue;

					queueInfo.queueFamilyIndex = i;
					queueInfo.pQueuePriorities = &queueSchedulingPriorities;

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

			VkResult result = device.create(physicalDevices[physicalDeviceIndex], &createInfo);

			if (result != VK_SUCCESS)
				throw std::runtime_error("FAILURE: device_creation");

			surface = std::move(l_surface);
			assignedQueueFamilies = std::move(l_assignedFamilies);
			assignedPresentFamily = l_assignedPresentFamily;
		}

		VmaAllocatorCreateInfo createInfo{};
		createInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
		createInfo.physicalDevice = r_selectedDevice;
		createInfo.device = device;
		createInfo.instance = instance;
		createInfo.vulkanApiVersion = VK_API_VERSION_1_3;

		allocator.create(&createInfo);
	}
		
}
