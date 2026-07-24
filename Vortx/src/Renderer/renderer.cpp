#include "renderer.h"

#include <array>
#include <vector>
#include <stdexcept>

#include <vulkan/vulkan_win32.h>

constexpr uint32_t INVALID_QUEUE_FAMILY = UINT32_MAX;

std::error_code Renderer::deploy() noexcept {
	std::error_code error;

	VkResult result;

	mem::stack _stash(16 << 20);

	VkInstance _instance = VK_NULL_HANDLE;
	mem::span<PhysicalDevice> _physicalDevices;

	do {

		mem::scratch.mark();

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
		}

		if (result != VK_SUCCESS) {
			error = make_error_code(RCode::NOT_SUPPORTED);
			break;
		}

		uint32_t physicalDeviceCount;

		result = vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr);

		if (result != VK_SUCCESS) {
			error = std::make_error_code(std::errc::not_supported);
			break;
		}

		_physicalDevices = _stash.alloc<PhysicalDevice>(physicalDeviceCount);

		mem::span<VkPhysicalDevice> devicehandles = mem::scratch.alloc<VkPhysicalDevice>(physicalDeviceCount);
		
		result = vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, devicehandles);
		
		if (result != VK_SUCCESS) {
			error = make_error_code(RCode::HARDWARE_INCAPABILITY);
			break;
		}

		for (uint32_t i = 0; i < physicalDeviceCount; ++i) {
			VkPhysicalDevice handle = devicehandles[i];

			vkGetPhysicalDeviceProperties(handle, &_physicalDevices[i].properties);
			vkGetPhysicalDeviceMemoryProperties(handle, &_physicalDevices[i].memory);
			vkGetPhysicalDeviceFeatures(handle, &_physicalDevices[i].features);

			_physicalDevices[i].handle = handle;
		}

		mem::scratch.restore();

		if(instance)
			vkDestroyInstance(instance, nullptr);

		stash = std::move(_stash);
		
		physicalDevices = _physicalDevices;

		instance = _instance;

		return {};

	} while (false);

	mem::scratch.restore();

	if(_instance)
		vkDestroyInstance(_instance, nullptr);

	return error;
}

void Renderer::enumeratePhysicalDeviceNames(size_t* count, const char** const deviceNames) noexcept {
	const size_t physicalDeviceCount = physicalDevices.size();

	if (count)
		*count = physicalDeviceCount;

	if (!deviceNames)
		return;

	for (size_t i = 0; i < physicalDeviceCount; ++i) {
		deviceNames[i] = physicalDevices[i].properties.deviceName;
	}
}

std::error_code Renderer::createDevice(HINSTANCE hinstance, HWND hwnd, size_t physicalDeviceIndex) {
	std::error_code error{};
	
	VkSurfaceKHR _surface = VK_NULL_HANDLE;

	VkPhysicalDevice physicalDevice = physicalDevices[physicalDeviceIndex].handle;

	VkDevice _device = VK_NULL_HANDLE;
	VmaAllocator _allocator = VK_NULL_HANDLE;

	VkBuffer _stagingBuffer = VK_NULL_HANDLE;
	VmaAllocation _stagingAllocation;

	mem::span<uint8_t> _stagingSpan;

	do {

		VkResult result;

		mem::scratch.mark();

		{
			VkWin32SurfaceCreateInfoKHR createinfo{};
			createinfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			createinfo.pNext = nullptr;
			createinfo.flags = 0;
			createinfo.hinstance = hinstance;
			createinfo.hwnd = hwnd;

			result = vkCreateWin32SurfaceKHR(instance, &createinfo, nullptr, &_surface);
		}

		if (result != VK_SUCCESS) {
			error = std::make_error_code(std::errc::not_supported);
			break;
		}

		mem::static_vector<uint32_t> uniqueQueueFamilies = mem::scratch.alloc<uint32_t>(3);

		QueueFamilyIndices _familyIndex = { INVALID_QUEUE_FAMILY, INVALID_QUEUE_FAMILY, INVALID_QUEUE_FAMILY };

		{
			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

			mem::span<VkQueueFamilyProperties> queueFamilies = mem::scratch.alloc<VkQueueFamilyProperties>(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

			std::vector<uint32_t> familyRoles(queueFamilyCount, 0);

			constexpr VkQueueFlags releventCapabilities = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				VkQueueFlags queueFlags = queueFamilies[i].queueFlags & releventCapabilities;
				familyRoles[i] = __popcnt(queueFlags);
			}

			uint32_t min = UINT32_MAX;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				if((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
					continue;

				if (familyRoles[i] >= min)
					continue;

				min = familyRoles[i];
				_familyIndex.graphics = i;
				uniqueQueueFamilies.push_back_unique(i);
			}

			min = UINT32_MAX;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				if ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) == 0)
					continue;

				if (familyRoles[i] >= min)
					continue;

				min = familyRoles[i];
				_familyIndex.transfer = i;
				uniqueQueueFamilies.push_back_unique(i);
			}

			VkBool32 presentSupport = VK_FALSE;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, _surface, &presentSupport);
				if (!presentSupport)
					continue;
				
				_familyIndex.present = i;
				uniqueQueueFamilies.push_back_unique(i);
			}
		}

		if (uniqueQueueFamilies.empty()) {
			error = make_error_code(RCode::HARDWARE_INCAPABILITY);
			break;
		}

		{
			const size_t queueInfoCount = uniqueQueueFamilies.size();

			mem::span<VkDeviceQueueCreateInfo> queueInfos = mem::scratch.alloc<VkDeviceQueueCreateInfo>(queueInfoCount);

			constexpr float queuePriority = 1.0f;

			for (size_t i = 0; i < queueInfoCount; ++i) {
				VkDeviceQueueCreateInfo* pInfo = &queueInfos[i];

				pInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				pInfo->pNext = nullptr;
				pInfo->flags = 0;
				pInfo->queueFamilyIndex = uniqueQueueFamilies[i];
				pInfo->queueCount = 1;
				pInfo->pQueuePriorities = &queuePriority;
			}

			constexpr std::array<const char*, 1> extensions{
				"VK_KHR_swapchain"
			};

			VkPhysicalDeviceFeatures enabledFeatures{};

			if (physicalDevices[physicalDeviceIndex].features.samplerAnisotropy)
				enabledFeatures.samplerAnisotropy = VK_TRUE;

			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
			createInfo.pQueueCreateInfos = queueInfos;
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();
			createInfo.pEnabledFeatures = &enabledFeatures;

			result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &_device);
		}

		if (result != VK_SUCCESS) {
			error = make_error_code(RCode::FAILURE);
			break;
		}

		DeviceQueues _deviceQueue{};

		vkGetDeviceQueue(_device, _familyIndex.graphics, 0, &_deviceQueue.graphics);
		vkGetDeviceQueue(_device, _familyIndex.transfer, 0, &_deviceQueue.transfer);
		vkGetDeviceQueue(_device, _familyIndex.present, 0, &_deviceQueue.present);

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

		if (result != VK_SUCCESS) {
			error = make_error_code(RCode::FAILURE);
			break;
		}

		{
			VkBufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.size = VkDeviceSize(64) << 20;
			createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo allocationCreateInfo{};
			allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
			allocationCreateInfo.requiredFlags = 0;
			allocationCreateInfo.preferredFlags = 0;
			allocationCreateInfo.memoryTypeBits = 0;
			allocationCreateInfo.pool = VK_NULL_HANDLE;
			allocationCreateInfo.pUserData = nullptr;
			allocationCreateInfo.priority = 0.0f;

			VmaAllocationInfo allocationInfo;

			result = vmaCreateBuffer(_allocator, &createInfo, &allocationCreateInfo, &_stagingBuffer, &_stagingAllocation, &allocationInfo);

			_stagingSpan = mem::span<uint8_t>{ reinterpret_cast<uint8_t*>(allocationInfo.pMappedData), static_cast<size_t>(allocationInfo.size) };
		}

		if (result != VK_SUCCESS) {
			error = make_error_code(RCode::FAILURE);
			break;
		}

		{
			rndr::PresentationStageCreateInfo createInfo{};
			createInfo.graphicsFamilyIndex = graphicsFamilyIndex;
			createInfo.graphicsQueue = graphicsQueue;
			createInfo.presentFamilyIndex = presentFamilyIndex;
			createInfo.presentQueue = presentQueue;

			result = presentationStage.root(_device, physicalDevice, _surface, &createInfo);
		}

		if (result != VK_SUCCESS)
			break;

		if (stagingBuffer)
			vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);

		if (allocator)
			vmaDestroyAllocator(allocator);

		if (device)
			vkDestroyDevice(device, nullptr);

		if (surface)
			vkDestroySurfaceKHR(instance, surface, nullptr);

		surface = _surface;

		device = _device;

		familyIndex = _familyIndex;
		deviceQueue = _deviceQueue;

		allocator = _allocator;

		transferStage = rndr::TransferStage{ _stagingSpan };

		return error;

	} while (false);

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
		result = presentationStage.targetSurface(surface, &targetIndex);
		if (result != VK_SUCCESS) {
			vkDestroySurfaceKHR(instance, surface, nullptr);

			return -1;
		}
	}

	result = presentationStage.configurePresentation(targetIndex, 2);
	if (result != VK_SUCCESS) {
		vkDestroySurfaceKHR(instance, surface, nullptr);

		return -1;
	}

	renderTargets.push_back(targetIndex);

	return 1;
}

int Renderer::createImage(const void* const pData, size_t _Size, uint32_t _Width, uint32_t _Height) noexcept {
	VkResult result;

	VkImage _image = VK_NULL_HANDLE;
	VmaAllocation _allocation = VK_NULL_HANDLE;

	do {
		{
			VkImageCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.imageType = VK_IMAGE_TYPE_2D;
			createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
			createInfo.extent = { _Width, _Height, 1 };
			createInfo.mipLevels = 1;
			createInfo.arrayLayers = 1;
			createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			createInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
			createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			VmaAllocationCreateInfo allocationInfo{};
			allocationInfo.flags = 0;
			allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
			allocationInfo.requiredFlags = 0;
			allocationInfo.preferredFlags = 0;
			allocationInfo.memoryTypeBits = 0;
			allocationInfo.pool = nullptr;
			allocationInfo.pUserData = nullptr;
			allocationInfo.priority = 0;

			result = vmaCreateImage(allocator, &createInfo, &allocationInfo, &_image, &_allocation, nullptr);
		}

		if (result != VK_SUCCESS)
			break;

	} while (false);

	if(_allocation)
		vmaDestroyImage(allocator, _image, _allocation);
	
	return -1;
}

void Renderer::reset() noexcept {
	{
		VkSurfaceKHR surface;

		const size_t renderTargetCount = renderTargets.size();

		for (size_t i = 0; i < renderTargetCount; ++i) {
			presentationStage.releaseSurface(renderTargets[i], &surface);
			vkDestroySurfaceKHR(instance, surface, nullptr);
		}
	}

	presentationStage.reset();

	if (allocator)
		vmaDestroyAllocator(allocator);

	if (device)
		vkDestroyDevice(device, nullptr);
}

Renderer::~Renderer() noexcept {
	if (instance)
		vkDestroyInstance(instance, nullptr);
}
