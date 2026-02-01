#include "instance_builder.h"

#include "Signboard/RHI/core/instance_vk.h"

#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <vector>

namespace {

	const std::vector<VkExtensionProperties>& availableExtensions() {
		static const std::vector<VkExtensionProperties> extensions = [] {

			uint32_t count = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

			std::vector<VkExtensionProperties> v(count);
			vkEnumerateInstanceExtensionProperties(nullptr, &count, v.data());
			return v;
			}();

		return extensions;
	}

	const std::vector<VkLayerProperties>& availableLayers() {
		static const std::vector<VkLayerProperties> layers = [] {

			uint32_t count = 0;
			vkEnumerateInstanceLayerProperties(&count, nullptr);

			std::vector<VkLayerProperties> v(count);
			vkEnumerateInstanceLayerProperties(&count, v.data());
			return v;
			}();

		return layers;
	}

}

namespace rhi::procedure {

	instance_builder::instance_builder() = default;

	instance_builder& instance_builder::addExtension(const char* name) {

		if (!extensionSupported(name))
			throw std::runtime_error(std::string("extension not supported: ") + name);
		
		m_extensions.push_back(name);

		return *this;
	}

	instance_builder& instance_builder::enableValidationLayer() {

		const char* valLayer = "VK_LAYER_KHRONOS_validation";

		if (!layerSupported(valLayer))
			throw std::runtime_error(std::string("validation not supported: ") + valLayer);
		
		m_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		m_layers.push_back(valLayer);

		return *this;
	}
	
	rhi::core::instance instance_builder::build() {

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "MyEngine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "MyEngine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_extensions.size());
		createInfo.ppEnabledExtensionNames = m_extensions.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_layers.size());
		createInfo.ppEnabledLayerNames = m_layers.data();

		VkInstance vk_instance = VK_NULL_HANDLE;
		if (vkCreateInstance(&createInfo, nullptr, &vk_instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vulkan instance!");
		}

		rhi::core::instance instance;
		instance.m_instance = vk_instance;

		return instance;
	}

	bool instance_builder::extensionSupported(const char* extName) {

		for (const auto& extension : availableExtensions()) {
			if (strcmp(extName, extension.extensionName) == 0)
				return true;
		}

		return false;
	}

	bool instance_builder::layerSupported(const char* layerName) {

		for (const auto& layer : availableLayers()) {
			if (strcmp(layerName, layer.layerName) == 0)
				return true;
		}

		return false;
	}

}
