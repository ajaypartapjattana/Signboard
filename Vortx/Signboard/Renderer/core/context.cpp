#include "context.hh"

namespace rndr {

	void context::createInstance() {
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
		rhi::surface l_surface{ instance, window };
		rhi::physicalDevice& r_selectedDevice = physicalDevices[physicalDeviceIndex];

		std::array<uint32_t, FAMILY_INDEX_MAX_ENUM> l_assignedFamilies{};
		l_assignedFamilies.fill(UINT32_MAX);

		uint32_t l_assignedPresentFamily = UINT32_MAX;

		uint32_t deviceFamilyCount;
		{
			sgb::span<const VkQueueFamilyProperties> queueFamilyProperties = r_selectedDevice.queueFamilyProperties();
			deviceFamilyCount = static_cast<uint32_t>(queueFamilyProperties.size());

			std::array<uint32_t, FAMILY_INDEX_MAX_ENUM> familyCapabilities{};
			familyCapabilities.fill(UINT32_MAX);

			constexpr VkQueueFlags relevantFamilyCapabilityMask = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
			constexpr std::array<VkQueueFlags, FAMILY_INDEX_MAX_ENUM> familyFlags{ VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT };

			for (uint32_t i = 0; i < deviceFamilyCount; ++i) {
				const VkQueueFamilyProperties& familyProperty = queueFamilyProperties[i];

				uint32_t queueFamilyCapabilityCount = __popcnt(familyProperty.queueFlags & relevantFamilyCapabilityMask);

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

			sgb::span<const VkBool32> familyPresentationSupport = r_selectedDevice.queueFamilyPresentationSupport(l_surface);

			uint32_t graphicsFamilyIndex = l_assignedFamilies[FAMILY_INDEX_GRAPHICS];
			if (familyPresentationSupport[graphicsFamilyIndex]) {
				l_assignedPresentFamily = graphicsFamilyIndex;
			}
			else {
				for (uint32_t i = 0; i < deviceFamilyCount; ++i) {
					if (familyPresentationSupport[i]) {
						l_assignedPresentFamily = i;
						break;
					}
				}
			}

			if(l_assignedPresentFamily == UINT32_MAX)
				throw std::runtime_error("INCAPABILITY: physical_device_does_not_support_presentation!");
		}

		{
			float queueSchedulingPriorities = 1.0f;
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

			{
				std::vector<uint8_t> requiredFamilies(deviceFamilyCount, false);

				for (uint32_t i = 0; i < FAMILY_INDEX_MAX_ENUM; ++i) {
					uint32_t familyIndex = l_assignedFamilies[i];
					requiredFamilies[familyIndex] = true;
				}

				requiredFamilies[l_assignedPresentFamily] = true;

				VkDeviceQueueCreateInfo queueInfo{};
				queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueInfo.pNext = nullptr;
				queueInfo.flags = 0;
				queueInfo.queueCount = 1;
				queueInfo.pQueuePriorities = &queueSchedulingPriorities;

				queueCreateInfos.reserve(deviceFamilyCount);

				for (uint32_t i = 0; i < deviceFamilyCount; ++i) {
					if (requiredFamilies[i] == false)
						continue;

					queueInfo.queueFamilyIndex = i;

					queueCreateInfos.emplace_back(queueInfo);
				}

			}

			constexpr std::array deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
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
			
		}

		VmaAllocatorCreateInfo createInfo{};
		createInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
		createInfo.physicalDevice = r_selectedDevice;
		createInfo.device = device;
		createInfo.instance = instance;
		createInfo.vulkanApiVersion = VK_API_VERSION_1_3;

		VkResult result = allocator.create(&createInfo);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: allocator_creation!");

		if (pPage) {
			pPage->surface = surface;
			pPage->physicalDevice = r_selectedDevice;
			pPage->device = device;
			pPage->assignedQueueFamilies = std::move(l_assignedFamilies);
			pPage->assignedPresentFamily = l_assignedPresentFamily;
			pPage->allocator = allocator;
		}
	}
		
}
