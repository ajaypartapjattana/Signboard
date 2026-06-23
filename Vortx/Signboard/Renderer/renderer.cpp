#include "renderer.h"

#include <array>
#include <vector>
#include <stdexcept>

#include <vulkan/vulkan_win32.h>

void Renderer::deploy() {
	VkResult result;

	VkInstance instance;

	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = "My Application";
		appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		appInfo.pEngineName = "My Engine";
		appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

#ifdef _DEBUG
		constexpr std::array<const char*, 1> instanceLayers{
			"VK_LAYER_KHRONOS_validation"
		};

		constexpr std::array<const char*, 3> instanceExtensions{
			"VK_KHR_surface",
			"VK_KHR_win32_surface",
			"VK_EXT_debug_utils"
		};

#else
		constexpr std::array<const char*, 0> instanceLayers{

		};

		constexpr std::array<const char*, 2> instanceExtensions{
			"VK_KHR_surface",
			"VK_KHR_win32_surface"
		};

#endif

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
		createInfo.ppEnabledLayerNames = instanceLayers.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		createInfo.ppEnabledExtensionNames = instanceExtensions.data();

		result = vkCreateInstance(&createInfo, nullptr, &instance);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE : instance_creation!");
	}

	std::vector<PhysicalDevice> physicalDevices;

	{
		uint32_t physicalDeviceCount;

		VkResult result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
		if (result != VK_SUCCESS) {
			vkDestroyInstance(instance, nullptr);

			throw std::runtime_error("FAILURE : physical_device_enumeration!");
		}

		std::vector<VkPhysicalDevice> physicalDeviceHandles(physicalDeviceCount);

		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDeviceHandles.data());

		physicalDevices.reserve(physicalDeviceCount);

		for (uint32_t i = 0; i < physicalDeviceCount; ++i) {
			VkPhysicalDevice handle = physicalDeviceHandles[i];

			PhysicalDevice& device = physicalDevices.emplace_back();

			vkGetPhysicalDeviceProperties(handle, &device.properties);
			vkGetPhysicalDeviceMemoryProperties(handle, &device.memory);
			vkGetPhysicalDeviceFeatures(handle, &device.features);

			device.handle = handle;
		}
	}

	if (m_instance) {
		vkDestroyInstance(m_instance, nullptr);
	}

	m_physicalDevices = std::move(physicalDevices);

	m_instance = instance;
}

void Renderer::enumeratePhysicalDevices(size_t* count, const char** deviceNames) noexcept {
	const size_t physicalDeviceCount = m_physicalDevices.size();

	if (count)
		*count = physicalDeviceCount;

	if (!deviceNames)
		return;

	for (size_t i = 0; i < physicalDeviceCount; ++i) {
		deviceNames[i] = m_physicalDevices[i].properties.deviceName;
	}
}

void Renderer::createDevice(HINSTANCE hinstance, HWND hwnd, size_t physicalDeviceIndex) {
	VkResult result;
	
	VkSurfaceKHR surface;
	
	{
		VkWin32SurfaceCreateInfoKHR createinfo{};
		createinfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createinfo.pNext = nullptr;
		createinfo.flags = 0;
		createinfo.hinstance = hinstance;
		createinfo.hwnd = hwnd;

		result = vkCreateWin32SurfaceKHR(m_instance, &createinfo, nullptr, &surface);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE : win32_surface_creation!");
	}

	VkPhysicalDevice physicalDevice = m_physicalDevices[physicalDeviceIndex].handle;

	uint32_t graphicsFamilyIndex = UINT32_MAX;
	VkQueue graphicsQueue;

	uint32_t transferFamilyIndex = UINT32_MAX;
	VkQueue transferQueue;

	uint32_t presentFamilyIndex = UINT32_MAX;
	VkQueue presentQueue;

	VkDevice device;

	{
		uint32_t queueFamilyCount;

		{
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

			std::vector<uint32_t> familyRoles(queueFamilyCount, 0);

			constexpr VkQueueFlags releventCapabilities = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				VkQueueFlags queueFlags = queueFamilies[i].queueFlags & releventCapabilities;
				familyRoles[i] = __popcnt(queueFlags);
			}

			{
				uint32_t min = UINT32_MAX;

				for (uint32_t i = 0; i < queueFamilyCount; ++i) {
					if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && familyRoles[i] < min) {
						min = familyRoles[i];
						graphicsFamilyIndex = i;
					}
				}

				if (graphicsFamilyIndex == UINT32_MAX) {
					vkDestroySurfaceKHR(m_instance, surface, nullptr);

					throw std::runtime_error("FAILURE : device_does_not_support_grpahics");
				}
			}

			{
				uint32_t min = UINT32_MAX;

				for (uint32_t i = 0; i < queueFamilyCount; ++i) {
					if ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && familyRoles[i] < min) {
						min = familyRoles[i];
						transferFamilyIndex = i;
					}
				}

				if (transferFamilyIndex == UINT32_MAX) {
					vkDestroySurfaceKHR(m_instance, surface, nullptr);

					throw std::runtime_error("FAILURE : device_does_not_support_transfer");
				}
			}

			VkBool32 presentSupport = VK_FALSE;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
				if (presentSupport) {
					presentFamilyIndex = i;
					break;
				}
			}

			if (presentFamilyIndex == UINT32_MAX) {
				vkDestroySurfaceKHR(m_instance, surface, nullptr);

				throw std::runtime_error("FAILURE : device_does_not_support_presentation");
			}
		}

		std::vector<VkDeviceQueueCreateInfo> queueInfos;

		{
			std::vector<VkBool32> requiredQueueFamilies(queueFamilyCount, VK_FALSE);

			requiredQueueFamilies[graphicsFamilyIndex] = VK_TRUE;
			requiredQueueFamilies[transferFamilyIndex] = VK_TRUE;
			requiredQueueFamilies[presentFamilyIndex] = VK_TRUE;

			queueInfos.reserve(queueFamilyCount);

			constexpr float queuePriority = 1.0f;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				if (!requiredQueueFamilies[i])
					continue;

				VkDeviceQueueCreateInfo& info = queueInfos.emplace_back();

				info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				info.pNext = nullptr;
				info.flags = 0;
				info.queueFamilyIndex = i;
				info.queueCount = 1;
				info.pQueuePriorities = &queuePriority;
			}

		}

		constexpr std::array<const char*, 1> extensions{
			"VK_KHR_swapchain"
		};

		VkPhysicalDeviceFeatures enabledFeatures{};

		if (m_physicalDevices[physicalDeviceIndex].features.samplerAnisotropy)
			enabledFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
		createInfo.pQueueCreateInfos = queueInfos.data();
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.pEnabledFeatures = &enabledFeatures;

		result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
		if (result != VK_SUCCESS) {
			vkDestroySurfaceKHR(m_instance, surface, nullptr);

			throw std::runtime_error("FAILURE : device_creation!");
		}

		vkGetDeviceQueue(device, graphicsFamilyIndex, 0, &graphicsQueue);
		vkGetDeviceQueue(device, transferFamilyIndex, 0, &transferQueue);
		vkGetDeviceQueue(device, presentFamilyIndex, 0, &presentQueue);
	}

	VmaAllocator allocator;

	{
		VmaAllocatorCreateInfo createInfo{};
		createInfo.flags = 0;
		createInfo.physicalDevice = physicalDevice;
		createInfo.device = device;
		createInfo.preferredLargeHeapBlockSize = 0;
		createInfo.pAllocationCallbacks = nullptr;
		createInfo.pDeviceMemoryCallbacks = nullptr;
		createInfo.pHeapSizeLimit = nullptr;
		createInfo.pVulkanFunctions = nullptr;
		createInfo.instance = m_instance;
		createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		createInfo.pTypeExternalMemoryHandleTypes = nullptr;

		result = vmaCreateAllocator(&createInfo, &allocator);
		if (result != VK_SUCCESS) {
			vkDestroyDevice(device, nullptr);
			vkDestroySurfaceKHR(m_instance, surface, nullptr);

			throw std::runtime_error("FAILURE : allocator_creation!");
		}
	}

	{
		rndr::PresentationStageQueueInfo queueInfo{};
		queueInfo.graphicsFamilyIndex = graphicsFamilyIndex;
		queueInfo.graphicsQueue = graphicsQueue;
		queueInfo.presentFamilyIndex = presentFamilyIndex;
		queueInfo.presentQueue = presentQueue;

		result = presentation.root(device, physicalDevice, surface, &queueInfo);
		if (result != VK_SUCCESS) {
			vkDestroyDevice(device, nullptr);
			vkDestroySurfaceKHR(m_instance, surface, nullptr);
			
			throw std::runtime_error("FAILURE : presentation_stage_initialization!");
		}
	}

	vkDestroySurfaceKHR(m_instance, surface, nullptr);

	if (m_allocator)
		vmaDestroyAllocator(m_allocator);

	if (m_device)
		vkDestroyDevice(m_device, nullptr);

	m_device = device;

	execution.graphicsFamilyIndex = graphicsFamilyIndex;
	execution.graphicsQueue = graphicsQueue;
	execution.transferFamilyIndex = transferFamilyIndex;
	execution.transferQueue = transferQueue;
	execution.presentFamilyIndex = presentFamilyIndex;
	execution.presentQueue = presentQueue;

	{
		VkSurfaceKHR presentationSurface;

		const size_t renderTargetCount = renderTargets.size();

		for (size_t i = 0; i < renderTargetCount; ++i) {
			presentation.releaseSurface(renderTargets[i], &presentationSurface);
			vkDestroySurfaceKHR(m_instance, presentationSurface, nullptr);
		}
	}
}

int Renderer::pushRenderTarget(HINSTANCE hinstance, HWND hwnd) noexcept {
	VkResult result;

	VkSurfaceKHR surface;

	{
		VkWin32SurfaceCreateInfoKHR createinfo{};
		createinfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createinfo.pNext = nullptr;
		createinfo.flags = 0;
		createinfo.hinstance = hinstance;
		createinfo.hwnd = hwnd;

		result = vkCreateWin32SurfaceKHR(m_instance, &createinfo, nullptr, &surface);
		if (result != VK_SUCCESS)
			return -1;
	}

	size_t targetIndex;

	{
		result = presentation.targetSurface(surface, &targetIndex);
		if (result != VK_SUCCESS) {
			vkDestroySurfaceKHR(m_instance, surface, nullptr);

			return -1;
		}
	}

	result = presentation.configurePresentation(targetIndex, 2);
	if (result != VK_SUCCESS) {
		vkDestroySurfaceKHR(m_instance, surface, nullptr);

		return -1;
	}

	renderTargets.push_back(targetIndex);

	return 1;
}

void Renderer::reset() noexcept {
	{
		VkSurfaceKHR surface;

		const size_t renderTargetCount = renderTargets.size();

		for (size_t i = 0; i < renderTargetCount; ++i) {
			presentation.releaseSurface(renderTargets[i], &surface);
			vkDestroySurfaceKHR(m_instance, surface, nullptr);
		}
	}

	presentation.reset();

	if (m_allocator)
		vmaDestroyAllocator(m_allocator);

	if (m_device)
		vkDestroyDevice(m_device, nullptr);
}

Renderer::~Renderer() noexcept {
	if (m_instance)
		vkDestroyInstance(m_instance, nullptr);
}
