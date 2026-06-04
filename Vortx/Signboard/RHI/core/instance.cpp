#include "instance.hh"

namespace rhi {

	VkResult instance::create(const VkInstanceCreateInfo* pCreateInfo) noexcept {

#ifdef _VALIDATE
		
		{
			VkResult result;

			uint32_t extensionCount = 0;
			result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> extensions(extensionCount);
			result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

			if (result != VK_SUCCESS)
				return result;

			for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; ++i) {
				const char* requested = pCreateInfo->ppEnabledExtensionNames[i];

				bool found = false;
				for (uint32_t j = 0; j < extensionCount; ++j) {
					if (strcmp(requested, extensions[j].extensionName))
						continue;
					
					found = true;
					break;
				}

				if (found)
					continue;

				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}

		{
			VkResult result;

			uint32_t layerCount = 0;
			result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			std::vector<VkLayerProperties> layers(layerCount);
			result = vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

			if (result != VK_SUCCESS)
				return result;

			for (uint32_t i = 0; i < pCreateInfo->enabledLayerCount; ++i) {
				const char* requested = pCreateInfo->ppEnabledLayerNames[i];

				bool found = false;
				for (uint32_t j = 0; j < layerCount; ++j) {
					if (strcmp(requested, layers[j].layerName))
						continue;

					found = true;
					break;
				}

				if (found)
					continue;

				return VK_ERROR_LAYER_NOT_PRESENT;
			}
		}

#endif

		VkInstance instance;
		VkResult result = vkCreateInstance(pCreateInfo, nullptr, &instance);
		if (result != VK_SUCCESS)
			return result;

		reset();
		m_instance = instance;

		return VK_SUCCESS;
	}

	void instance::reset() noexcept {
		if (m_instance)
			vkDestroyInstance(m_instance, nullptr);

		m_instance = VK_NULL_HANDLE;
	}

	std::vector<VkPhysicalDevice> instance::enumeratePhysicalDevices() const {
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		VkResult result = vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data());
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: physical_device_enumeration!");

		return physicalDevices;
	}

}
