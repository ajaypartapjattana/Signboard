#include "renderer.h"

#include <array>
#include <vector>
#include <stdexcept>

#include <vulkan/vulkan_win32.h>

void Renderer::deploy() {
	VkResult result;

	VkInstance _instance;

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

		result = vkCreateInstance(&createInfo, nullptr, &_instance);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE : instance_creation!");
	}

	std::vector<PhysicalDevice> physicalDevices;

	{
		uint32_t physicalDeviceCount;

		VkResult result = vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr);
		if (result != VK_SUCCESS) {
			vkDestroyInstance(_instance, nullptr);

			throw std::runtime_error("FAILURE : physical_device_enumeration!");
		}

		std::vector<VkPhysicalDevice> physicalDeviceHandles(physicalDeviceCount);

		vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, physicalDeviceHandles.data());

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

	if (instance) {
		vkDestroyInstance(instance, nullptr);
	}

	m_physicalDevices = std::move(physicalDevices);

	instance = _instance;
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
	
	VkPhysicalDevice physicalDevice = m_physicalDevices[physicalDeviceIndex].handle;
	
	VkSurfaceKHR _surface = VK_NULL_HANDLE;

	uint32_t graphicsFamilyIndex = UINT32_MAX;
	VkQueue graphicsQueue;

	uint32_t transferFamilyIndex = UINT32_MAX;
	VkQueue transferQueue;

	uint32_t presentFamilyIndex = UINT32_MAX;
	VkQueue presentQueue;

	VkDevice _device = VK_NULL_HANDLE;

	VmaAllocator _allocator = VK_NULL_HANDLE;

	do {
		{
			VkWin32SurfaceCreateInfoKHR createinfo{};
			createinfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			createinfo.pNext = nullptr;
			createinfo.flags = 0;
			createinfo.hinstance = hinstance;
			createinfo.hwnd = hwnd;

			result = vkCreateWin32SurfaceKHR(instance, &createinfo, nullptr, &_surface);
		}

		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE : win32_surface_creation!");

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

			uint32_t min = UINT32_MAX;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && familyRoles[i] < min) {
					min = familyRoles[i];
					graphicsFamilyIndex = i;
				}
			}

			min = UINT32_MAX;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				if ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && familyRoles[i] < min) {
					min = familyRoles[i];
					transferFamilyIndex = i;
				}
			}

			VkBool32 presentSupport = VK_FALSE;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, _surface, &presentSupport);
				if (presentSupport) {
					presentFamilyIndex = i;
					break;
				}
			}
		}

		if (graphicsFamilyIndex == UINT32_MAX || transferFamilyIndex == UINT32_MAX || presentFamilyIndex == UINT32_MAX)
			break;
		
		{
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

			result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &_device);
		}

		if (result != VK_SUCCESS)
			break;

		vkGetDeviceQueue(_device, graphicsFamilyIndex, 0, &graphicsQueue);
		vkGetDeviceQueue(_device, transferFamilyIndex, 0, &transferQueue);
		vkGetDeviceQueue(_device, presentFamilyIndex, 0, &presentQueue);

		{
			VmaAllocatorCreateInfo createInfo{};
			createInfo.flags = 0;
			createInfo.physicalDevice = physicalDevice;
			createInfo.device = _device;
			createInfo.preferredLargeHeapBlockSize = 0;
			createInfo.pAllocationCallbacks = nullptr;
			createInfo.pDeviceMemoryCallbacks = nullptr;
			createInfo.pHeapSizeLimit = nullptr;
			createInfo.pVulkanFunctions = nullptr;
			createInfo.instance = instance;
			createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
			createInfo.pTypeExternalMemoryHandleTypes = nullptr;

			result = vmaCreateAllocator(&createInfo, &_allocator);
		}

		if (result != VK_SUCCESS)
			break;

		{
			rndr::PresentationStageQueueInfo queueInfo{};
			queueInfo.graphicsFamilyIndex = graphicsFamilyIndex;
			queueInfo.graphicsQueue = graphicsQueue;
			queueInfo.presentFamilyIndex = presentFamilyIndex;
			queueInfo.presentQueue = presentQueue;

			result = presentation.root(_device, physicalDevice, _surface, &queueInfo);
		}

		vkDestroySurfaceKHR(instance, _surface, nullptr);

	} while (false);

	if (result == VK_SUCCESS) {
		if (allocator)
			vmaDestroyAllocator(allocator);

		if (device)
			vkDestroyDevice(device, nullptr);

		device = _device;

		execution.graphicsFamilyIndex = graphicsFamilyIndex;
		execution.graphicsQueue = graphicsQueue;
		execution.transferFamilyIndex = transferFamilyIndex;
		execution.transferQueue = transferQueue;
		execution.presentFamilyIndex = presentFamilyIndex;
		execution.presentQueue = presentQueue;

		allocator = _allocator;
	}

	if (_allocator)
		vmaDestroyAllocator(_allocator);

	if (_device)
		vkDestroyDevice(_device, nullptr);

	if (_surface)
		vkDestroySurfaceKHR(instance, _surface, nullptr);

	throw std::runtime_error("FAILURE : device_creation!");
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

		result = vkCreateWin32SurfaceKHR(instance, &createinfo, nullptr, &surface);
		if (result != VK_SUCCESS)
			return -1;
	}

	size_t targetIndex;

	{
		result = presentation.targetSurface(surface, &targetIndex);
		if (result != VK_SUCCESS) {
			vkDestroySurfaceKHR(instance, surface, nullptr);

			return -1;
		}
	}

	result = presentation.configurePresentation(targetIndex, 2);
	if (result != VK_SUCCESS) {
		vkDestroySurfaceKHR(instance, surface, nullptr);

		return -1;
	}

	renderTargets.push_back(targetIndex);

	return 1;
}

int Renderer::createImageInstance(uint32_t width, uint32_t height) {
	VkResult result;

	VkImage image;

	VmaAllocation allocation;

	{
		VkImageCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.imageType = VK_IMAGE_TYPE_2D;
		createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		createInfo.extent = { width, height, 1 };
		createInfo.mipLevels = 1;
		createInfo.arrayLayers = 1;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.flags = 0;
		allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocationInfo.requiredFlags = 0;
		allocationInfo.preferredFlags = 0;
		allocationInfo.memoryTypeBits = 0;
		allocationInfo.pool = nullptr;
		allocationInfo.pUserData = nullptr;
		allocationInfo.priority = 0;

		result = vmaCreateImage(allocator, &createInfo, &allocationInfo, &image, &allocation, nullptr);
	}


}

void Renderer::reset() noexcept {
	{
		VkSurfaceKHR surface;

		const size_t renderTargetCount = renderTargets.size();

		for (size_t i = 0; i < renderTargetCount; ++i) {
			presentation.releaseSurface(renderTargets[i], &surface);
			vkDestroySurfaceKHR(instance, surface, nullptr);
		}
	}

	presentation.reset();

	if (allocator)
		vmaDestroyAllocator(allocator);

	if (device)
		vkDestroyDevice(device, nullptr);
}

Renderer::~Renderer() noexcept {
	if (instance)
		vkDestroyInstance(instance, nullptr);
}
