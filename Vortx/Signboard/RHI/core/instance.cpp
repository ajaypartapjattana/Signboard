#include "instance.h"

#include <stdexcept>

namespace rhi::core {

	instance::instance(const instance_CI& createInfo) noexcept
		: m_instance(VK_NULL_HANDLE)
	{
		build(createInfo);
	}

	instance::instance(instance&& other) noexcept 
		: m_instance(other.m_instance)
	{
		other.m_instance = VK_NULL_HANDLE;
	}

	instance& instance::operator=(instance&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_instance)
			vkDestroyInstance(m_instance, nullptr);

		m_instance = other.m_instance;

		other.m_instance = VK_NULL_HANDLE;

		return *this;
	}

	instance::~instance() noexcept {
		if (m_instance != VK_NULL_HANDLE)
			vkDestroyInstance(m_instance, nullptr);
	}

	const VkInstance instance::native_instance() const noexcept {
		return m_instance;
	}

	void instance::build(const instance_CI& ci) {
		std::vector<const char*> l_extensions = ci.extensions;
		std::vector<const char*> l_layers = ci.layers;

		if (ci.enable_validation) {
			l_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			l_layers.push_back("VK_LAYER_KHRONOS_validation");
		}

		for (const char* ext : l_extensions)
			if (!validateExtension(ext))
				throw std::runtime_error(std::string("INCOMPLETE: extension_invalid::") + ext);

		for (const char* layer : l_layers)
			if (!validateLayer(layer))
				throw std::runtime_error(std::string("INCOMPLETE: layer_invalid::") + layer);

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
		createInfo.enabledExtensionCount = static_cast<uint32_t>(l_extensions.size());
		createInfo.ppEnabledExtensionNames = l_extensions.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(l_layers.size());
		createInfo.ppEnabledLayerNames = l_layers.data();
		createInfo.pNext = nullptr;
		createInfo.flags = 0;

		VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: instance_creation!");

	}

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

	bool instance::validateExtension(const char* extName) {

		for (const auto& extension : availableExtensions()) {
			if (strcmp(extName, extension.extensionName) == 0)
				return true;
		}

		return false;
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

	bool instance::validateLayer(const char* layerName) {

		for (const auto& layer : availableLayers()) {
			if (strcmp(layerName, layer.layerName) == 0)
				return true;
		}

		return false;
	}

}
