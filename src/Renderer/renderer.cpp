#include <array>
#include <algorithm>
#include <vector>

#if defined(WINDOW_WIN32)
	#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(WINDOW_XCB)
	#define VK_USE_PLATFORM_XCB_KHR
#elif defined(WINDOW_WAYLAND)
	#define VK_USE_PLATFORM_WAYLAND_KHR
#elif defined(WINDOW_X11)
	#define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <core/io/io.h>
#include <core/util.h>

#include "renderer.h"

constexpr uint32_t INVALID_QUEUE_FAMILY = UINT32_MAX;

struct PhysicalDeviceInfo {
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceMemoryProperties memory;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDevice handle;
};

struct VulkanContext_T {
	VkInstance instance;
	mem::span<PhysicalDeviceInfo> deviceInfo;
};

int requestVulkanContext(mem::stack* const pScratch, VulkanContext* const pContext) noexcept {
	VkInstance _instance = VK_NULL_HANDLE;

	mem::span<PhysicalDeviceInfo> _deviceInfo;
	
	do {
		VkResult result;

		pScratch->mark();
		
	#if defined(_DEBUG)
		constexpr std::array<const char*, 1> instanceLayers{
			"VK_LAYER_KHRONOS_validation"
		};
	#else
		constexpr std::array<const char*, 0> instanceLayers{

		};
	#endif

	#if defined(WINDOW_WIN32)
	  #if defined(_DEBUG)
		constexpr std::array<const char*, 3> instanceExtensions{
			"VK_KHR_surface",
			"VK_KHR_win32_surface",
			"VK_EXT_debug_utils"
		};
	  #else
		constexpr std::array<const char*, 2> instanceExtensions{
			"VK_KHR_surface",
			"VK_KHR_win32_surface"
		};
	  #endif
	#elif defined(WINDOW_XCB)
	  #if defined(_DEBUG)
		constexpr std::array<const char*, 3> instanceExtensions{
			"VK_KHR_surface",
			"VK_KHR_xcb_surface",
			"VK_EXT_debug_utils"
		};
	  #else
		constexpr std::array<const char*, 2> instanceExtensions{
			"VK_KHR_surface",
			"VK_KHR_xcb_surface"
		};
	  #endif
	#elif defined(WINDOW_WAYLAND)
	  #if defined(_DEBUG)
		constexpr std::array<const char*, 3> instanceExtensions{
			"VK_KHR_surface",
			"VK_KHR_wayland_surface",
			"VK_EXT_debug_utils"
		};
	  #else
		constexpr std::array<const char*, 2> instanceExtensions{
			"VK_KHR_surface",
			"VK_KHR_wayland_surface"
		};
	  #endif
	#elif defined(WINDOW_X11)
	  #if defined(_DEBUG)
		constexpr std::array<const char*, 3> instanceExtensions{
			"VK_KHR_surface",
			"VK_KHR_xlib_surface",
			"VK_EXT_debug_utils"
		};
	  #else
		constexpr std::array<const char*, 2> instanceExtensions{
			"VK_KHR_surface",
			"VK_KHR_xlib_surface"
		};
	  #endif
	#endif

		{
			VkApplicationInfo appInfo{};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pNext = nullptr;
			appInfo.pApplicationName = "My Application";
			appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			appInfo.pEngineName = "My Engine";
			appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_3;

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

		if (result != VK_SUCCESS)
			break;

		uint32_t physicalDeviceCount;

		result = vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr);

		if (result != VK_SUCCESS)
			break;

		_deviceInfo = mem::allocate_range<PhysicalDeviceInfo>((size_t)physicalDeviceCount);

		if (!_deviceInfo)
			break;

		mem::span<VkPhysicalDevice> physicalDevice = pScratch->alloc<VkPhysicalDevice>(physicalDeviceCount);

		if (!physicalDevice)
			break;

		result = vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, physicalDevice);
		
		if (result != VK_SUCCESS)
			break;

		PhysicalDeviceInfo* pDeviceInfo = _deviceInfo.pBegin;

		const VkPhysicalDevice* const pPhysicalDeviceEnd = physicalDevice.pEnd;
		for (const VkPhysicalDevice* pPhysicalDevice{ physicalDevice.pBegin }; pPhysicalDevice != pPhysicalDeviceEnd;) {
			const VkPhysicalDevice device = *pPhysicalDevice++;

			vkGetPhysicalDeviceProperties(device, &pDeviceInfo->properties);
			vkGetPhysicalDeviceMemoryProperties(device, &pDeviceInfo->memory);
			vkGetPhysicalDeviceFeatures(device, &pDeviceInfo->features);

			pDeviceInfo++->handle = device;
		}
		
		VulkanContext const context = new(std::nothrow) VulkanContext_T;
		
		if (!context)
			break;
		
		pScratch->restore();

		context->instance = _instance;
		context->deviceInfo = _deviceInfo;
		
		*pContext = context;
		
		return 0;

	} while (false);

	pScratch->restore();

	if (_deviceInfo)
		mem::free_range(_deviceInfo);

	if (_instance)
		vkDestroyInstance(_instance, nullptr);

	return -1;
}

void destroyVulkanContext(VulkanContext const _Context) noexcept {
	mem::free_range(_Context->deviceInfo);
	vkDestroyInstance(_Context->instance, nullptr);

	delete _Context;
}

int enumeratePhysicalDevices(VulkanContext const _Context, uint32_t* const pCount, const char** const pDeviceNames) noexcept {
	if (!pDeviceNames) {
		*pCount = static_cast<uint32_t>(_Context->deviceInfo.size());
		return 0;
	}

	const char** pDeviceName = pDeviceNames;

	const PhysicalDeviceInfo* const pPhysicalDeviceEnd = _Context->deviceInfo.pBegin + *pCount;
	for(const PhysicalDeviceInfo* pDevice{ _Context->deviceInfo.pBegin }; pDevice != pPhysicalDeviceEnd; ++pDevice) {
		*pDeviceName++ = pDevice->properties.deviceName;
	}

	return 0;
}

struct QueueFamilyIndices {
	uint32_t graphics;
	uint32_t transfer;
	uint32_t present;
};

struct DeviceQueues {
	VkQueue graphics;
	VkQueue transfer;
	VkQueue present;
};

struct Emulator_T {
	VkInstance instance;
	VkPhysicalDevice physicalDevice;

	VkSurfaceKHR surface;
	VkDevice device;
	QueueFamilyIndices queueFamily;
	DeviceQueues queue;
	
	VmaAllocator allocator;
};

int createEmulator(VulkanContext const _Context, const EmulatorCreateInfo* const pCreateInfo, mem::stack* const pScratch, Emulator* const pEmulator) noexcept {
	const VkInstance instance = _Context->instance;
	const PhysicalDeviceInfo* const pPhysicalDeviceInfo = _Context->deviceInfo + pCreateInfo->physicalDevice;
	
	const VkPhysicalDevice physicalDevice = pPhysicalDeviceInfo->handle;
	
	VkSurfaceKHR _surface = VK_NULL_HANDLE;
	VkDevice _device = VK_NULL_HANDLE;
	VmaAllocator _allocator = VK_NULL_HANDLE;

	do {
		VkResult result;

		pScratch->mark();
		
	  #if defined(WINDOW_WIN32)
		{
			VkWin32SurfaceCreateInfoKHR createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.connection = reinterpret_cast<HINSTANCE>(pCreateInfo->windowContext);
			createInfo.window = static_cast<HWND>(pCreateInfo->windowHandle);

			result = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &_surface);
		}
	  #elif defined(WINDOW_XCB)
		{
			VkXcbSurfaceCreateInfoKHR createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.connection = reinterpret_cast<xcb_connection_t*>(pCreateInfo->windowContext);
			createInfo.window = static_cast<xcb_window_t>(pCreateInfo->windowHandle);

			result = vkCreateXcbSurfaceKHR(instance, &createInfo, nullptr, &_surface);
		}
	  #elif defined(WINDOW_WAYLAND)
	  	{
			VkXcbSurfaceCreateInfoKHR createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.connection = reinterpret_cast<xcb_connection_t*>(pCreateInfo->windowContext);
			createInfo.window = static_cast<xcb_window_t>(pCreateInfo->windowHandle);

			result = vkCreateXcbSurfaceKHR(instance, &createInfo, nullptr, &_surface);
		}
	  #elif defined(WINDOW_X11)
	  	{
			VkXcbSurfaceCreateInfoKHR createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.connection = reinterpret_cast<xcb_connection_t*>(pCreateInfo->windowContext);
			createInfo.window = static_cast<xcb_window_t>(pCreateInfo->windowHandle);

			result = vkCreateXcbSurfaceKHR(instance, &createInfo, nullptr, &_surface);
		}
	  #endif

		if (result != VK_SUCCESS)
			break;

		mem::static_vector<uint32_t> uniqueQueueFamilies = pScratch->alloc<uint32_t>(3u);

		QueueFamilyIndices queueFamily = { INVALID_QUEUE_FAMILY, INVALID_QUEUE_FAMILY, INVALID_QUEUE_FAMILY };

		{
			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

			mem::span<VkQueueFamilyProperties> queueFamilies = pScratch->alloc<VkQueueFamilyProperties>(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

			mem::span<uint32_t> familyRole = pScratch->alloc<uint32_t>(queueFamilyCount);
			familyRole.assign_default();

			constexpr VkQueueFlags releventCapabilities = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				VkQueueFlags queueFlags = queueFamilies[i].queueFlags & releventCapabilities;
				familyRole[i] = intrin_popCount32(queueFlags);
			}

			uint32_t min = UINT32_MAX;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				if((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
					continue;

				if (familyRole[i] >= min)
					continue;

				min = familyRole[i];
				queueFamily.graphics = i;
				uniqueQueueFamilies.push_back_unique(i);
			}

			min = UINT32_MAX;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				if ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) == 0)
					continue;

				if (familyRole[i] >= min)
					continue;

				min = familyRole[i];
				queueFamily.transfer = i;
				uniqueQueueFamilies.push_back_unique(i);
			}

			VkBool32 presentSupport = VK_FALSE;

			for (uint32_t i = 0; i < queueFamilyCount; ++i) {
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, _surface, &presentSupport);
				if (!presentSupport)
					continue;
					
				queueFamily.present = i;
				uniqueQueueFamilies.push_back_unique(i);

				break;
			}
		}

		if (uniqueQueueFamilies.empty())
			break;
			
		{
			const size_t queueInfoCount = uniqueQueueFamilies.size();

			mem::span<VkDeviceQueueCreateInfo> queueInfos = pScratch->alloc<VkDeviceQueueCreateInfo>(queueInfoCount);

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

			if (pPhysicalDeviceInfo->features.samplerAnisotropy)
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

		if (result != VK_SUCCESS)
			break;

		DeviceQueues deviceQueue;

		vkGetDeviceQueue(_device, queueFamily.graphics, 0, &deviceQueue.graphics);
		vkGetDeviceQueue(_device, queueFamily.transfer, 0, &deviceQueue.transfer);
		vkGetDeviceQueue(_device, queueFamily.present, 0, &deviceQueue.present);

		{
			VmaAllocatorCreateInfo createInfo{};
			createInfo.flags = VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT;
			createInfo.physicalDevice = physicalDevice;
			createInfo.device = _device;
			createInfo.preferredLargeHeapBlockSize = 0;
			createInfo.pAllocationCallbacks = nullptr;
			createInfo.pDeviceMemoryCallbacks = nullptr;
			createInfo.pHeapSizeLimit = nullptr;
			createInfo.pVulkanFunctions = nullptr;
			createInfo.instance = _Context->instance;
			createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
			createInfo.pTypeExternalMemoryHandleTypes = nullptr;

			result = vmaCreateAllocator(&createInfo, &_allocator);
		}

		if (result != VK_SUCCESS)
			break;

		Emulator const emulator = new(std::nothrow) Emulator_T;

		if (!emulator)
			break;

		pScratch->restore();

		emulator->allocator = _allocator;

		emulator->queue = deviceQueue;
		emulator->queueFamily = queueFamily;
		emulator->device = _device;
		emulator->surface = _surface;
		
		emulator->physicalDevice = physicalDevice;
		emulator->instance = instance;

		*pEmulator = emulator;

		return 0;

	} while (false);

	pScratch->restore();

	if (_allocator)
		vmaDestroyAllocator(_allocator);

	if (_device)
		vkDestroyDevice(_device, nullptr);

	if (_surface)
		vkDestroySurfaceKHR(_Context->instance, _surface, nullptr);

	return -1;
}

void destroyEmulator(Emulator const _Emulator) noexcept {
	vmaDestroyAllocator(_Emulator->allocator);

	vkDestroyDevice(_Emulator->device, nullptr);

	vkDestroySurfaceKHR(_Emulator->instance, _Emulator->surface, nullptr);

	delete _Emulator;
}

int waitEmulator(Emulator const _Emulator) noexcept {
	const VkDevice device = _Emulator->device;

	if (vkDeviceWaitIdle(device) == VK_SUCCESS)
		return 0;

	return -1;
}

struct StagingState {
	mem::span<uint8_t> stage;
	uint8_t* pHead;
};

static inline void bindStageMemory(StagingState* const pStage, void* const pMemory, const size_t _Size) noexcept {
	pStage->stage = mem::span<uint8_t>{ reinterpret_cast<uint8_t*>(pMemory), _Size };
	pStage->pHead = reinterpret_cast<uint8_t*>(pMemory);
}

static inline size_t getStageSize(const StagingState* const pStage) noexcept {
	return pStage->stage.size();
}

static inline size_t getStageCapacity(const StagingState* const pStage) noexcept {
	return static_cast<size_t>(pStage->stage.pEnd - pStage->pHead);
}

struct StageRegion {
	VkDeviceSize offset;
	VkDeviceSize size;
};

static inline void allocateStageRegion(StagingState* const pStage, const void* const pSrc, const size_t _Size, StageRegion* const pAllocation) noexcept {
	memcpy(pStage->pHead, pSrc, _Size);
	
	pAllocation->offset = static_cast<VkDeviceSize>(pStage->pHead - pStage->stage.pBegin);
	pAllocation->size = _Size;

	pStage->pHead += _Size;
}

static inline void resetStage(StagingState* const pStage) noexcept {
	pStage->pHead = pStage->stage.pBegin;
}

struct Loader_T {
	VkDevice device;
	VkQueue queue;
	VmaAllocator allocator;

	VkCommandPool commandPool;
	mem::span<VkCommandBuffer> commandBuffer;
	mem::span<VkSemaphore> semaphore;
	mem::span<VkFence> fence;
	VkBuffer buffer;
	VmaAllocation allocation;
	StagingState stage;
	mem::span<uint8_t*> region;

	uint32_t transfer;
};

int createLoader(Emulator const _Emulator, const LoaderCreateInfo* const pCreateInfo, Loader* const pAsyncLoader) noexcept {
	const VkDevice device = _Emulator->device;
	const VmaAllocator allocator = _Emulator->allocator;
	
	VkCommandPool _commandPool = VK_NULL_HANDLE;
	mem::span<VkCommandBuffer> _commandBuffer;
	mem::span<VkSemaphore> _sempahore;
	mem::span<VkFence> _fence;
	VkBuffer _buffer = VK_NULL_HANDLE;
	VmaAllocation _allocation;
	mem::span<uint8_t*> _region;

	do {
		VkResult result;

		{
			VkCommandPoolCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			createInfo.queueFamilyIndex = _Emulator->queueFamily.transfer;

			result = vkCreateCommandPool(device, &createInfo, nullptr, &_commandPool);
		}

		if (result != VK_SUCCESS)
			break;

		_commandBuffer = mem::allocate_range<VkCommandBuffer>(pCreateInfo->maxLoadProcess);

		if (!_commandBuffer)
			break;

		{
			VkCommandBufferAllocateInfo allocateInfo{};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = _commandPool;
			allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = pCreateInfo->maxLoadProcess;
			
			result = vkAllocateCommandBuffers(device, &allocateInfo, _commandBuffer);
		}

		if (result != VK_SUCCESS)
			break;

		_sempahore = mem::allocate_range<VkSemaphore>(pCreateInfo->maxLoadProcess);

		_sempahore.assign_default();

		{
			VkSemaphoreCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;

			const VkSemaphore* const pSemaphoreEnd = _sempahore.pEnd;
			for(VkSemaphore* pSemaphore{ _sempahore.pBegin }; pSemaphore != pSemaphoreEnd && result == VK_SUCCESS; ++pSemaphore)
				result = vkCreateSemaphore(device, &createInfo, nullptr, pSemaphore);
		}

		if (result != VK_SUCCESS)
			break;

		_fence = mem::allocate_range<VkFence>(pCreateInfo->maxLoadProcess);

		if (!_fence)
			break;

		_fence.assign_default();

		{
			VkFenceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			const VkFence* const pFenceEnd = _fence.pEnd;
			for (VkFence* pFence{ _fence.pBegin }; pFence != pFenceEnd && result == VK_SUCCESS; ++pFence)
				result = vkCreateFence(device, &createInfo, nullptr, pFence);
		}

		if (result != VK_SUCCESS)
			break;

		mem::span<uint8_t> stagingSpan;
			
		{
			VkBufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.size = VkDeviceSize(pCreateInfo->stageSize);
			createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo allocationCreateInfo{};
			allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
			allocationCreateInfo.requiredFlags = 0;
			allocationCreateInfo.preferredFlags = 0;
			allocationCreateInfo.memoryTypeBits = 0;
			allocationCreateInfo.pool = VK_NULL_HANDLE;
			allocationCreateInfo.pUserData = nullptr;
			allocationCreateInfo.priority = 0.0f;

			VmaAllocationInfo allocationInfo;
			result = vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, &_buffer, &_allocation, &allocationInfo);

			stagingSpan = mem::span<uint8_t>{ reinterpret_cast<uint8_t*>(allocationInfo.pMappedData), static_cast<size_t>(allocationInfo.size) };
		}

		if (result != VK_SUCCESS)
			break;

		_region = mem::allocate_range<uint8_t*>(pCreateInfo->maxLoadProcess);

		if (!_region)
			break;

		_region.assign_default();
		
		Loader const loader = new(std::nothrow) Loader_T;

		if (!loader)
			break;

		loader->transfer = 0;
		loader->stage.stage = stagingSpan;
		loader->stage.pHead = stagingSpan.pBegin;
		loader->allocation = _allocation;
		loader->buffer = _buffer;
		loader->fence = _fence;
		loader->semaphore = _sempahore;
		loader->commandBuffer = _commandBuffer;
		loader->commandPool = _commandPool;
		
		loader->allocator = allocator;
		loader->queue = _Emulator->queue.transfer;
		loader->device = device;
		
		*pAsyncLoader = loader;

		return 0;

	} while (false);

	if (_region)
		mem::free_range(_region);

	if (_buffer)
		vmaDestroyBuffer(allocator, _buffer, _allocation);

	if (_fence) {
		const VkFence* const pFenceEnd = _fence.pEnd;
		for (const VkFence* pFence{ _fence.pBegin }; pFence != pFenceEnd && *pFence; ++pFence)
			vkDestroyFence(device, *pFence, nullptr);

		mem::free_range(_fence);
	}

	if (_sempahore) {
		const VkSemaphore* const pSemaphoreEnd = _sempahore.pEnd;
		for (const VkSemaphore* pSemaphore{ _sempahore.pBegin }; pSemaphore != pSemaphoreEnd && *pSemaphore; ++pSemaphore)
			vkDestroySemaphore(device, *pSemaphore, nullptr);

		mem::free_range(_sempahore);
	}

	if (_commandBuffer)
		mem::free_range(_commandBuffer);

	if (_commandPool)
		vkDestroyCommandPool(device, _commandPool, nullptr);

	return -1;
}

void destroyLoader(Loader const _AsynLoader) noexcept {
	mem::free_range(_AsynLoader->region);
	
	vmaDestroyBuffer(_AsynLoader->allocator, _AsynLoader->buffer, _AsynLoader->allocation);

	const VkFence* const pFenceEnd = _AsynLoader->fence.pEnd;
	for (const VkFence* pFence{ _AsynLoader->fence.pBegin }; pFence != pFenceEnd; ++pFence)
		vkDestroyFence(_AsynLoader->device, *pFence, nullptr);

	mem::free_range(_AsynLoader->fence);

	const VkSemaphore* const pSemaphoreEnd = _AsynLoader->semaphore.pEnd;
	for (const VkSemaphore* pSemaphore{ _AsynLoader->semaphore.pBegin }; pSemaphore != pSemaphoreEnd; ++pSemaphore)
		vkDestroySemaphore(_AsynLoader->device, *pSemaphore, nullptr);

	mem::free_range(_AsynLoader->semaphore);

	mem::free_range(_AsynLoader->commandBuffer);
	vkDestroyCommandPool(_AsynLoader->device, _AsynLoader->commandPool, nullptr);
	
	delete _AsynLoader;
}

int waitLoader(Loader const _AsyncLoader) noexcept {
	do {
		uint32_t transfer = _AsyncLoader->transfer;

		if (!transfer)
			return 0;

		VkResult result = vkWaitForFences(_AsyncLoader->device, transfer, _AsyncLoader->fence, VK_TRUE, UINT64_MAX);

		if (result != VK_SUCCESS)
			break;

		return 0;

	} while (false);

	return -1;
}

struct Model_T {
	uint32_t firstVertex;
	uint32_t firstIndex;
	uint32_t indexcount;
};

struct Collection_T {
	VmaAllocator allocator;
	mem::span<Model_T> model;
	VkBuffer vertex;
	VmaAllocation vertexAllocation;
	VkBuffer index;
	VmaAllocation indexAllocation;
};

int createCollection(Loader const _AsyncLoader, Emulator const _Emulator, const CollectionCreateInfo* const pCreateInfo, ProcessCookie* const pProcessCookie, Collection* const pScene) noexcept {
	const VkDevice device = _AsyncLoader->device;
	const VmaAllocator allocator = _AsyncLoader->allocator;
	
	mem::span<Model_T> _model;

	VkBuffer _vertex = VK_NULL_HANDLE;
	VmaAllocation _vertexAllocation;
	VkBuffer _index = VK_NULL_HANDLE;
	VmaAllocation _indexAllocation;

	do {
		VkResult result;
		
		const uint32_t modelCount = pCreateInfo->modelCount;

		if (!modelCount)
			break;

		_model = mem::allocate_range<Model_T>((size_t)modelCount);

		if (!_model)
			break;

		uint32_t totalVertexCount = 0;
		uint32_t totalIndexCount = 0;
			
		uint32_t maxVertexCount = 0;
		uint32_t maxIndexCount = 0;

		Model_T* pModel = _model.pBegin;

		const ModelInfo const* pModelInfoEnd = pCreateInfo->pModelInfos + pCreateInfo->modelCount;
		for (const ModelInfo* pModelInfo{ pCreateInfo->pModelInfos }; pModelInfo != pModelInfoEnd; ++pModelInfo) {
			pModel->firstVertex = totalVertexCount;
			pModel->firstIndex = totalIndexCount;

			pModel->indexcount = pModelInfo->indexCount;

			totalVertexCount += pModelInfo->vertexCount;
			totalIndexCount += pModelInfo->indexCount;

			maxVertexCount = std::max<uint32_t>(pModelInfo->vertexCount, maxVertexCount);
			maxIndexCount = std::max<uint32_t>(pModelInfo->indexCount, maxIndexCount);

			++pModel;
		}

		const size_t maxAllocationSize = sizeof(Vertex) * maxVertexCount + sizeof(Index) * maxIndexCount;

		if (maxAllocationSize > getStageSize(&_AsyncLoader->stage)) {
			VkBuffer _buffer = VK_NULL_HANDLE;
			VmaAllocation _allocation;

			VkBufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.size = maxAllocationSize;
			createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo allocationCreateInfo{};
			allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
			allocationCreateInfo.requiredFlags = 0;
			allocationCreateInfo.preferredFlags = 0;
			allocationCreateInfo.memoryTypeBits = 0;
			allocationCreateInfo.pool = VK_NULL_HANDLE;
			allocationCreateInfo.pUserData = nullptr;
			allocationCreateInfo.priority = 0.0f;

			VmaAllocationInfo allocationInfo;
			result = vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, &_buffer, &_allocation, &allocationInfo);

			if (result != VK_SUCCESS)
				break;

			if (_AsyncLoader->transfer) {
				result = vkWaitForFences(device, _AsyncLoader->transfer, _AsyncLoader->fence, VK_TRUE, UINT64_MAX);

				if (result != VK_SUCCESS) {
					vmaDestroyBuffer(allocator, _buffer, _allocation);
					break;
				}
			}

			_AsyncLoader->transfer = 0;

			_AsyncLoader->buffer = _buffer;
			_AsyncLoader->allocation = _allocation;
			
			bindStageMemory(&_AsyncLoader->stage, allocationInfo.pMappedData, (size_t)allocationInfo.size);
		}

		const VkDeviceSize vertexBufferSize = (VkDeviceSize)(sizeof(Vertex) * totalVertexCount);
		
		{
			VkBufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.size = vertexBufferSize;
			createInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0u;
			createInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo allocationCreateInfo{};
			allocationCreateInfo.flags = 0;
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
			allocationCreateInfo.requiredFlags = 0;
			allocationCreateInfo.preferredFlags = 0;
			allocationCreateInfo.memoryTypeBits = 0;
			allocationCreateInfo.pool = VK_NULL_HANDLE;
			allocationCreateInfo.pUserData = nullptr;
			allocationCreateInfo.priority = 0.0f;

			result = vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, &_vertex, &_vertexAllocation, nullptr);
		}

		if (result != VK_SUCCESS)
			break;

		const VkDeviceSize indexBufferSize = (VkDeviceSize)(sizeof(Index) * totalIndexCount);

		{
			VkBufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.size = indexBufferSize;
			createInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0u;
			createInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo allocationCreateInfo{};
			allocationCreateInfo.flags = 0;
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
			allocationCreateInfo.requiredFlags = 0;
			allocationCreateInfo.preferredFlags = 0;
			allocationCreateInfo.memoryTypeBits = 0;
			allocationCreateInfo.pool = VK_NULL_HANDLE;
			allocationCreateInfo.pUserData = nullptr;
			allocationCreateInfo.priority = 0.0f;

			result = vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, &_index, &_indexAllocation, nullptr);
		}

		if (result != VK_SUCCESS)
			break;

		VkDeviceSize vertexOffset = 0;
		VkDeviceSize indexOffset = 0;

		const ModelInfo* pModelInfo = pCreateInfo->pModelInfos;
		while (pModelInfo != pModelInfoEnd) {
			uint32_t transfer = _AsyncLoader->transfer;

			while (true) {
				result = vkGetFenceStatus(device, _AsyncLoader->fence[transfer]);

				if (result == VK_NOT_READY) {
					++transfer;
					break;
				}

				if (result != VK_SUCCESS)
					break;

				uint8_t* region = _AsyncLoader->region[transfer];
				
				if (!region)
					region = _AsyncLoader->stage.pHead;

				_AsyncLoader->stage.pHead = region;
				
				if (transfer == 0)
					break;

				--transfer;
			}

			if (result != VK_SUCCESS && result != VK_NOT_READY)
				break;

			if (transfer == _AsyncLoader->fence.size()) {
				result = vkWaitForFences(device, static_cast<uint32_t>(_AsyncLoader->fence.size()), _AsyncLoader->fence, VK_TRUE, UINT64_MAX);

				if (result != VK_SUCCESS)
					break;

				resetStage(&_AsyncLoader->stage);
			}

			const VkFence fence = _AsyncLoader->fence[transfer];
			
			result = vkWaitForFences(device, 1u, &fence, VK_TRUE, UINT64_MAX);

			if (result != VK_SUCCESS)
				break;

			const VkCommandBuffer commandBuffer = _AsyncLoader->commandBuffer[_AsyncLoader->transfer];

			result = vkResetCommandBuffer(commandBuffer, 0);

			if (result != VK_SUCCESS)
				break;

			{
				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.pNext = nullptr;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
				beginInfo.pInheritanceInfo = nullptr;

				result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
			}

			if (result != VK_SUCCESS)
				break;

			StagingState* const pStage = &_AsyncLoader->stage;

			do {
				uint8_t* const pHead = pStage->pHead;
				
				const size_t vertexSize = sizeof(Vertex) * pModelInfo->vertexCount;
				const size_t indexSize = sizeof(Index) * pModelInfo->indexCount;
				
				const size_t allocationSize = vertexSize + indexSize;
				
				if (getStageCapacity(pStage) < allocationSize)
					break;
				
				StageRegion vertexRegion;
				allocateStageRegion(pStage, pModelInfo->pVertex, vertexSize, &vertexRegion);

				StageRegion indexRegion;
				allocateStageRegion(pStage, pModelInfo->pIndex, indexSize, &indexRegion);

				result = vmaFlushAllocation(_AsyncLoader->allocator, _AsyncLoader->allocation, vertexRegion.offset, allocationSize);

				if (result != VK_SUCCESS) {
					pStage->pHead = pHead;
					break;
				}

				{
					VkBufferCopy copy{};
					copy.srcOffset = vertexRegion.offset;
					copy.dstOffset = vertexOffset;
					copy.size = vertexRegion.size;

					vkCmdCopyBuffer(commandBuffer, _AsyncLoader->buffer, _vertex, 1u, &copy);
				}

				vertexOffset += vertexRegion.size;

				{
					VkBufferCopy copy{};
					copy.srcOffset = indexRegion.offset;
					copy.dstOffset = indexOffset;
					copy.size = indexRegion.size;

					vkCmdCopyBuffer(commandBuffer, _AsyncLoader->buffer, _index, 1u, &copy);
				}

				indexOffset += indexRegion.size;

				++pModelInfo;
			} while (pModelInfo != pModelInfoEnd);

			if (result != VK_SUCCESS)
				break;

			result = vkEndCommandBuffer(commandBuffer);

			if (result != VK_SUCCESS)

			result = vkResetFences(device, 1u, &fence);

			if (result != VK_SUCCESS)
				break;

			{
				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.pNext = nullptr;
				submitInfo.waitSemaphoreCount = 0u;
				submitInfo.pWaitSemaphores = nullptr;
				submitInfo.pWaitDstStageMask = nullptr;
				submitInfo.commandBufferCount = 1u;
				submitInfo.pCommandBuffers = &commandBuffer;
				submitInfo.signalSemaphoreCount = 0u;
				submitInfo.pSignalSemaphores = nullptr;

				result = vkQueueSubmit(_AsyncLoader->queue, 1u, &submitInfo, fence);
			}

			if (result != VK_SUCCESS)
				break;

			_AsyncLoader->region[transfer] = _AsyncLoader->stage.pHead;
		}

		if (result != VK_SUCCESS)
			break;

		Collection const scene = new(std::nothrow) Collection_T;

		if (!scene)
			break;

		scene->indexAllocation = _indexAllocation;
		scene->index = _index;
		scene->vertexAllocation = _vertexAllocation;
		scene->vertex = _vertex;
		scene->model = _model;
		scene->allocator = _Emulator->allocator;

		*pScene = scene;

		return 0;

	} while (false);

	if (_index)
		vmaDestroyBuffer(allocator, _index, _indexAllocation);

	if (_vertex)
		vmaDestroyBuffer(allocator, _vertex, _vertexAllocation);

	if (_model)
		mem::free_range(_model);

	return -1;
}

void destroyCollection(Collection const _Collection) noexcept {
	const VmaAllocator allocator = _Collection->allocator;

	vmaDestroyBuffer(allocator, _Collection->index, _Collection->indexAllocation);
	vmaDestroyBuffer(allocator, _Collection->vertex, _Collection->vertexAllocation);

	mem::free_range(_Collection->model);

	delete _Collection;
}

struct Surface_T {
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue queue;

	VkSurfaceFormatKHR surfaceFormat;
	VkPresentModeKHR presentMode;

	VkExtent2D extent;
	uint32_t layers;
	VkSwapchainKHR swapchain;
	mem::span<VkImage> image;
	mem::span<VkImageView> imageView;
	mem::span<VkFence> fence;
};

int createSurface(Emulator const _Emulator, const SurfaceCreateInfo* const pCreateInfo, mem::stack* const pScratch, Surface* const pCanvas) noexcept {
	const VkSurfaceKHR surface = _Emulator->surface;
	const VkPhysicalDevice physicalDevice = _Emulator->physicalDevice;
	const VkDevice device = _Emulator->device;

	VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
	mem::span<VkImage> _image;
	mem::span<VkImageView> _imageView;
	mem::span<VkFence> _fence;

	do {
		VkResult result;

		pScratch->mark();

		VkSurfaceFormatKHR surfaceFormat;

		{
			constexpr VkSurfaceFormatKHR preferredSurfaceFormats[] {
				{ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
				{ VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
				{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
				{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }
			};

			uint32_t surfaceFormatCount;

			result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);

			if (result != VK_SUCCESS)
				break;

			mem::span<VkSurfaceFormatKHR> surfaceFormats = pScratch->alloc<VkSurfaceFormatKHR>(surfaceFormatCount);

			result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats);
				
			if (result != VK_SUCCESS)
				break;

			const VkSurfaceFormatKHR* pSurfaceFormat = nullptr;

			const VkSurfaceFormatKHR* const pFormatEnd = preferredSurfaceFormats + 4u;
			for (const VkSurfaceFormatKHR* pFormat{ preferredSurfaceFormats }; pFormat != pFormatEnd; ++pFormat) {
				pSurfaceFormat = std::find_if(surfaceFormats.pBegin, surfaceFormats.pEnd, [pFormat](const VkSurfaceFormatKHR& _Format) { return _Format.format == pFormat->format && _Format.colorSpace == pFormat->colorSpace; });

				if (pSurfaceFormat != surfaceFormats.pEnd)
					break;

				pSurfaceFormat = nullptr;
			}

			if (!pSurfaceFormat)
				break;

			surfaceFormat = *pSurfaceFormat;
		}

		VkPresentModeKHR presentMode;

		{
			uint32_t presentModeCount;

			result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

			if (result != VK_SUCCESS)
				break;

			mem::span<VkPresentModeKHR> presentModes = pScratch->alloc<VkPresentModeKHR>(presentModeCount);

			result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes);

			if (result != VK_SUCCESS)
				break;

			VkPresentModeKHR* pPresentMode = std::find(presentModes.pBegin, presentModes.pEnd, VK_PRESENT_MODE_MAILBOX_KHR);

			presentMode = pPresentMode != presentModes.pEnd ? *pPresentMode : VK_PRESENT_MODE_FIFO_KHR;
		}

		VkSurfaceCapabilitiesKHR surfaceCapabilities;

		result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

		if (result != VK_SUCCESS)
			break;

		uint32_t minImageCount = pCreateInfo->minImageCount;

		{
			minImageCount = std::max<uint32_t>(minImageCount, surfaceCapabilities.minImageCount);

			if (surfaceCapabilities.maxImageCount)
				minImageCount = std::min<uint32_t>(minImageCount, surfaceCapabilities.maxImageCount);
		}

		{
			VkSwapchainCreateInfoKHR createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.surface = surface;
			createInfo.minImageCount = minImageCount;
			createInfo.imageFormat = surfaceFormat.format;
			createInfo.imageColorSpace = surfaceFormat.colorSpace;
			createInfo.imageExtent = surfaceCapabilities.currentExtent;
			createInfo.imageArrayLayers = 1u;
			createInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
			createInfo.preTransform = surfaceCapabilities.currentTransform;
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.presentMode = presentMode;
			createInfo.clipped = VK_TRUE;
			createInfo.oldSwapchain = VK_NULL_HANDLE;

			result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &_swapchain);
		}

		if (result != VK_SUCCESS)
			break;

		uint32_t imageCount;
			
		result = vkGetSwapchainImagesKHR(device, _swapchain, &imageCount, nullptr);

		if (result != VK_SUCCESS)
			break;

		_image = mem::allocate_range<VkImage>((size_t)imageCount);

		if (!_image)
			break;

		result = vkGetSwapchainImagesKHR(device, _swapchain, &imageCount, _image);
			
		if (result != VK_SUCCESS)
			break;

		_imageView = mem::allocate_range<VkImageView>((size_t)imageCount);

		if (!_imageView)
			break;

		_imageView.assign_default();

		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = surfaceFormat.format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VkImageView* pImageView = _imageView.pBegin;

			const VkImage* const pImageEnd = _image.pEnd;
			for (const VkImage* pImage{ _image.pBegin }; pImage != pImageEnd && result == VK_SUCCESS; ++pImage) {
				createInfo.image = *pImage;
				result = vkCreateImageView(device, &createInfo, nullptr, pImageView++);
			}
		}

		if (result != VK_SUCCESS)
			break;

		_fence = mem::allocate_range<VkFence>((size_t)imageCount);
		
		if (!_fence)
			break;

		_fence.assign_default();

		Surface const canvas = new(std::nothrow) Surface_T;

		if (!canvas)
			break;

		pScratch->restore();

		canvas->fence = _fence;
		canvas->imageView = _imageView;
		canvas->image = _image;
		canvas->swapchain = _swapchain;
		canvas->layers = 1u;
		canvas->extent = surfaceCapabilities.currentExtent;
		
		canvas->presentMode = presentMode;
		canvas->surfaceFormat = surfaceFormat;

		canvas->queue = _Emulator->queue.present;
		canvas->device = device;
		canvas->physicalDevice = physicalDevice;
		canvas->surface = surface;

		*pCanvas = canvas;

		return 0;

	} while (false);

	pScratch->restore();

	if (_fence)
		mem::free_range(_fence);	

	if (_imageView) {
		const VkImageView* const pImageViewEnd = _imageView.pEnd;
		for (const VkImageView* pImageView{ _imageView.pBegin }; pImageView != pImageViewEnd && *pImageView; ++pImageView)
			vkDestroyImageView(device, *pImageView, nullptr);

		mem::free_range(_imageView);
	}

	if (_image)
		mem::free_range(_image);

	if (_swapchain)
		vkDestroySwapchainKHR(device, _swapchain, nullptr);

	return -1;
}

void destroySurface(Surface const _Canvas) noexcept {
	const VkDevice device = _Canvas->device;

	mem::free_range(_Canvas->fence);

	const VkImageView* const pImageViewEnd = _Canvas->imageView.pEnd;
	for (const VkImageView* pImageView{ _Canvas->imageView.pBegin }; pImageView != pImageViewEnd; ++pImageView)
		vkDestroyImageView(device, *pImageView, nullptr);

	mem::free_range(_Canvas->imageView);
	mem::free_range(_Canvas->image);

	vkDestroySwapchainKHR(device, _Canvas->swapchain, nullptr);

	delete _Canvas;
}

int updateCanvas(Surface const _Canvas) noexcept {
	const VkSurfaceKHR surface = _Canvas->surface;
	const VkPhysicalDevice physicalDevice = _Canvas->physicalDevice;
	const VkDevice device = _Canvas->device;

	VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
	mem::span<VkImage> _image;
	mem::span<VkImageView> _imageView;
	mem::span<VkFence> _fence;

	do {
		VkResult result;
		
		VkSurfaceCapabilitiesKHR surfaceCapabilities;

		result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

		if (result != VK_SUCCESS)
			break;

		uint32_t minImageCount = static_cast<uint32_t>(_Canvas->image.size());

		{
			minImageCount = std::max<uint32_t>(minImageCount, surfaceCapabilities.minImageCount);

			if (surfaceCapabilities.maxImageCount)
				minImageCount = std::min<uint32_t>(minImageCount, surfaceCapabilities.maxImageCount);
		}

		{
			VkSwapchainCreateInfoKHR createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.surface = surface;
			createInfo.minImageCount = minImageCount;
			createInfo.imageFormat = _Canvas->surfaceFormat.format;
			createInfo.imageColorSpace = _Canvas->surfaceFormat.colorSpace;
			createInfo.imageExtent = surfaceCapabilities.currentExtent;
			createInfo.imageArrayLayers = 1u;
			createInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
			createInfo.preTransform = surfaceCapabilities.currentTransform;
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.presentMode = _Canvas->presentMode;
			createInfo.clipped = VK_TRUE;
			createInfo.oldSwapchain = _Canvas->swapchain;

			result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &_swapchain);
		}

		if (result != VK_SUCCESS)
			break;

		uint32_t imageCount;
		
		result = vkGetSwapchainImagesKHR(device, _swapchain, &imageCount, nullptr);

		if (result != VK_SUCCESS)
			break;

		_image = mem::allocate_range<VkImage>((size_t)imageCount);

		if (!_image)
			break;

		result = vkGetSwapchainImagesKHR(device, _swapchain, &imageCount, _image);

		if (result != VK_SUCCESS)
			break;

		_imageView = mem::allocate_range<VkImageView>((size_t)imageCount);

		if (!_imageView)
			break;

		_imageView.assign_default();

		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = _Canvas->surfaceFormat.format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VkImageView* pImageView = _imageView;

			const VkImage* const pImageEnd = _image.pEnd;
			for (const VkImage* pImage{ _image.pBegin }; pImage != pImageEnd && result == VK_SUCCESS; ++pImage){
				createInfo.image = *pImage;
				result = vkCreateImageView(device, &createInfo, nullptr, pImageView++);
			}
		}

		if (result != VK_SUCCESS)
			break;

		_fence = mem::allocate_range<VkFence>((size_t)imageCount);

		if (!_fence)
			break;

		_fence.assign_default();

		const VkFence* const pFenceEnd = _Canvas->fence.pEnd;
		for (const VkFence* pFence{ _Canvas->fence.pBegin }; pFence != pFenceEnd && result == VK_SUCCESS; ++pFence)
			result = *pFence ? vkWaitForFences(device, 1u, pFence, VK_TRUE, UINT64_MAX) : VK_SUCCESS;

		if (result != VK_SUCCESS)
			break;

		mem::free_range(_Canvas->fence);

		const VkImageView* const pImageViewEnd = _Canvas->imageView.pEnd;
		for (const VkImageView* pImageView{ _Canvas->imageView.pBegin }; pImageView != pImageViewEnd; ++pImageView)
			vkDestroyImageView(device, *pImageView, nullptr);

		mem::free_range(_Canvas->imageView);
		mem::free_range(_Canvas->image);

		vkDestroySwapchainKHR(device, _Canvas->swapchain, nullptr);

		_Canvas->fence = _fence;
		_Canvas->imageView = _imageView;
		_Canvas->image = _image;
		_Canvas->swapchain = _swapchain;
		_Canvas->extent = surfaceCapabilities.currentExtent;

		return 0;

	} while (false);

	if (_fence)
		mem::free_range(_fence);

	if (_imageView) {
		const VkImageView* const pImageViewEnd = _imageView.pEnd;
		for (const VkImageView* pImageView{ _imageView.pBegin }; pImageView != pImageViewEnd && *pImageView; ++pImageView)
			vkDestroyImageView(device, *pImageView, nullptr);

		mem::free_range(_imageView);
	}

	if (_image)
		mem::free_range(_image);

	if (_swapchain)
		vkDestroySwapchainKHR(device, _swapchain, nullptr);

	return -1;
}

struct RenderPass_T {
	VkDevice device;

	mem::span<VkClearValue> clearValue;
	VkRenderPass renderPass;
	VkDescriptorSetLayout cameraSetLayout;
	VkDescriptorSetLayout objectSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

	mem::span<VkFramebuffer> framebuffer;
	VkRect2D renderArea;
};

int createRenderPass(Emulator const _Emulator, const RenderPassCreateInfo* const pCreateInfo, mem::stack* const pScratch, RenderPass* const pRenderPass) noexcept {
	const VkDevice device = _Emulator->device;

	mem::span<VkClearValue> _clearValue;
	VkRenderPass _renderPass = VK_NULL_HANDLE;
	VkDescriptorSetLayout _cameraSetLayout = VK_NULL_HANDLE;
	VkDescriptorSetLayout _objectSetLayout = VK_NULL_HANDLE;
	VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
	VkPipeline _pipeline = VK_NULL_HANDLE;
	mem::span<VkFramebuffer> _framebuffer;

	do {
		VkResult result;

		_clearValue = mem::allocate_range<VkClearValue>((size_t)1u);

		_clearValue[0] = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };

		const VkFormat surfaceFormat = pCreateInfo->surface->surfaceFormat.format;

		{
			VkAttachmentDescription attachment{};
			attachment.format = surfaceFormat;
			attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference reference{};
			reference.attachment = 0;
			reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass{};
			subpass.flags = 0;
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.inputAttachmentCount = 0;
			subpass.pInputAttachments = nullptr;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &reference;
			subpass.pResolveAttachments = nullptr;
			subpass.pDepthStencilAttachment = nullptr;
			subpass.preserveAttachmentCount = 0;
			subpass.pPreserveAttachments = nullptr;

			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = VK_ACCESS_NONE;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependency.dependencyFlags = 0;

			VkRenderPassCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.attachmentCount = 1;
			createInfo.pAttachments = &attachment;
			createInfo.subpassCount = 1;
			createInfo.pSubpasses = &subpass;
			createInfo.dependencyCount = 1;
			createInfo.pDependencies = &dependency;

			result = vkCreateRenderPass(device, &createInfo, nullptr, &_renderPass);
		}

		if (result != VK_SUCCESS)
			break;

		{
			VkDescriptorSetLayoutBinding binding[1]{};
			binding[0].binding = 0;
			binding[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			binding[0].descriptorCount = 1u;
			binding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			binding[0].pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.bindingCount = 1u;
			createInfo.pBindings = binding;

			result = vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &_cameraSetLayout);
		}

		if (result != VK_SUCCESS)
			break;

		{
			VkDescriptorSetLayoutBinding binding[1]{};
			binding[0].binding = 1;
			binding[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			binding[0].descriptorCount = 1u;
			binding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			binding[0].pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.bindingCount = 1u;
			createInfo.pBindings = binding;

			result = vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &_objectSetLayout);
		}

		if (result != VK_SUCCESS)
			break;

		{
			const VkDescriptorSetLayout setLayouts[2] = {_cameraSetLayout, _objectSetLayout};

			VkPushConstantRange pushRange{};
			pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			pushRange.size = sizeof(uint32_t);
			pushRange.offset = 0u;

			VkPipelineLayoutCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.setLayoutCount = 2u;
			createInfo.pSetLayouts = setLayouts;
			createInfo.pushConstantRangeCount = 1u;
			createInfo.pPushConstantRanges = &pushRange;

			result = vkCreatePipelineLayout(device, &createInfo, nullptr, &_pipelineLayout);
		}

		if (result != VK_SUCCESS)
			break;

		{
			VkShaderModule vertex;

			{
				const char* const vertPath = "shaders/base.vert.spv";

				size_t size;

				int error = io::getBinarySize(vertPath, &size);

				if (error)
					break;

				pScratch->mark();

				mem::span<uint8_t> bin = pScratch->alloc<uint8_t>(size);

				error = io::loadBinary(vertPath, size, bin);

				if (error)
					break;

				VkShaderModuleCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				createInfo.codeSize = size;
				createInfo.pCode = reinterpret_cast<const uint32_t*>(bin.pBegin);

				result = vkCreateShaderModule(device, &createInfo, nullptr, &vertex);

				pScratch->restore();
			}

			if (result != VK_SUCCESS)
				break;

			VkShaderModule fragment;

			{
				const char* const fragPath = "shaders/base.frag.spv";

				size_t size;

				int error = io::getBinarySize(fragPath, &size);

				if (error) {
					vkDestroyShaderModule(device, vertex, nullptr);
					break;
				}

				pScratch->mark();

				mem::span<uint8_t> bin = pScratch->alloc<uint8_t>(size);

				error = io::loadBinary(fragPath, size, bin);

				if (error) {
					vkDestroyShaderModule(device, vertex, nullptr);
					break;
				}

				VkShaderModuleCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				createInfo.codeSize = size;
				createInfo.pCode = reinterpret_cast<const uint32_t*>(bin.pBegin);

				result = vkCreateShaderModule(device, &createInfo, nullptr, &fragment);

				pScratch->restore();
			}

			if (result != VK_SUCCESS) {
				vkDestroyShaderModule(device, vertex, nullptr);
				break;
			}

			VkPipelineShaderStageCreateInfo stageInfo[2]{};
			stageInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stageInfo[0].pNext = nullptr;
			stageInfo[0].flags = 0;
			stageInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
			stageInfo[0].module = vertex;
			stageInfo[0].pName = "main";
			stageInfo[0].pSpecializationInfo = nullptr;

			stageInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stageInfo[1].pNext = nullptr;
			stageInfo[1].flags = 0;
			stageInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			stageInfo[1].module = fragment;
			stageInfo[1].pName = "main";
			stageInfo[1].pSpecializationInfo = nullptr;

			VkVertexInputBindingDescription binding[1]{};
			binding[0].binding = 0;
			binding[0].stride = sizeof(Vertex);
			binding[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			VkVertexInputAttributeDescription attribute[2]{};
			attribute[0].binding = 0;
			attribute[0].location = 0;
			attribute[0].format = VK_FORMAT_R32G32_SFLOAT;
			attribute[0].offset = offsetof(Vertex, Vertex::pos);

			attribute[1].binding = 0;
			attribute[1].location = 1;
			attribute[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attribute[1].offset = offsetof(Vertex, Vertex::color);

			VkPipelineVertexInputStateCreateInfo vertexInputState{};
			vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputState.pNext = nullptr;
			vertexInputState.flags = 0;
			vertexInputState.vertexBindingDescriptionCount = 1;
			vertexInputState.pVertexBindingDescriptions = binding;
			vertexInputState.vertexAttributeDescriptionCount = 2u;
			vertexInputState.pVertexAttributeDescriptions = attribute;

			VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
			inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssemblyState.pNext = nullptr;
			inputAssemblyState.flags = 0;
			inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssemblyState.primitiveRestartEnable = VK_FALSE;

			VkPipelineTessellationStateCreateInfo tessellationState{};
			tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			tessellationState.pNext = nullptr;
			tessellationState.flags = 0;
			tessellationState.patchControlPoints = 3;

			VkPipelineViewportStateCreateInfo viewportState{};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.pNext = nullptr;
			viewportState.flags = 0;
			viewportState.viewportCount = 1;
			viewportState.pViewports = nullptr;
			viewportState.scissorCount = 1;
			viewportState.pScissors = nullptr;

			VkPipelineRasterizationStateCreateInfo rasterizationState{};
			rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizationState.pNext = nullptr;
			rasterizationState.flags = 0;
			rasterizationState.depthClampEnable = VK_FALSE;
			rasterizationState.rasterizerDiscardEnable = VK_FALSE;
			rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizationState.cullMode = VK_CULL_MODE_NONE;
			rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rasterizationState.depthBiasEnable = VK_FALSE;
			rasterizationState.depthBiasConstantFactor = 0.0f;
			rasterizationState.depthBiasClamp = 0.0f;
			rasterizationState.depthBiasSlopeFactor = 0.0f;
			rasterizationState.lineWidth = 1.0f;

			VkPipelineMultisampleStateCreateInfo multisampleState{};
			multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampleState.pNext = nullptr;
			multisampleState.flags = 0;
			multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampleState.sampleShadingEnable = VK_FALSE;
			multisampleState.minSampleShading = 1.0f;
			multisampleState.pSampleMask = nullptr;
			multisampleState.alphaToCoverageEnable = VK_FALSE;
			multisampleState.alphaToOneEnable = VK_FALSE;

			VkPipelineDepthStencilStateCreateInfo depthStencilState{};
			depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencilState.pNext = nullptr;
			depthStencilState.flags = 0;
			depthStencilState.depthTestEnable = VK_FALSE;
			depthStencilState.depthWriteEnable = VK_FALSE;
			depthStencilState.depthCompareOp = VK_COMPARE_OP_ALWAYS;
			depthStencilState.depthBoundsTestEnable = VK_FALSE;
			depthStencilState.stencilTestEnable = VK_FALSE;
			depthStencilState.front = {};
			depthStencilState.back = {};
			depthStencilState.minDepthBounds = 0.0f;
			depthStencilState.maxDepthBounds = 1.0f;

			VkPipelineColorBlendAttachmentState colorBlendAttachment{};
			colorBlendAttachment.blendEnable = VK_FALSE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

			VkPipelineColorBlendStateCreateInfo colorBlendState{};
			colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlendState.pNext = nullptr;
			colorBlendState.flags = 0;
			colorBlendState.logicOpEnable = VK_FALSE;
			colorBlendState.logicOp = VK_LOGIC_OP_COPY;
			colorBlendState.attachmentCount = 1;
			colorBlendState.pAttachments = &colorBlendAttachment;
			colorBlendState.blendConstants[0] = 0.0f;
			colorBlendState.blendConstants[1] = 0.0f;
			colorBlendState.blendConstants[2] = 0.0f;
			colorBlendState.blendConstants[3] = 0.0f;

			std::array dynamicStates{
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};

			VkPipelineDynamicStateCreateInfo dynamicState{};
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.pNext = nullptr;
			dynamicState.flags = 0;
			dynamicState.dynamicStateCount = 2;
			dynamicState.pDynamicStates = dynamicStates.data();

			VkGraphicsPipelineCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.stageCount = 2;
			createInfo.pStages = stageInfo;
			createInfo.pVertexInputState = &vertexInputState;
			createInfo.pInputAssemblyState = &inputAssemblyState;
			createInfo.pTessellationState = nullptr;
			createInfo.pViewportState = &viewportState;
			createInfo.pRasterizationState = &rasterizationState;
			createInfo.pMultisampleState = &multisampleState;
			createInfo.pDepthStencilState = &depthStencilState;
			createInfo.pColorBlendState = &colorBlendState;
			createInfo.pDynamicState = &dynamicState;
			createInfo.layout = _pipelineLayout;
			createInfo.renderPass = _renderPass;
			createInfo.subpass = 0;
			createInfo.basePipelineHandle = VK_NULL_HANDLE;
			createInfo.basePipelineIndex = 0;

			result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &_pipeline);

			vkDestroyShaderModule(device, fragment, nullptr);
			vkDestroyShaderModule(device, vertex, nullptr);
		}

		if (result != VK_SUCCESS)
			break;

		_framebuffer = mem::allocate_range<VkFramebuffer>(pCreateInfo->surface->imageView.size());

		if (!_framebuffer)
			break;

		_framebuffer.assign_default();

		const VkExtent2D extent = pCreateInfo->surface->extent;

		{
			VkFramebufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.renderPass = _renderPass;
			createInfo.attachmentCount = 1u;
			createInfo.width = extent.width;
			createInfo.height = extent.height;
			createInfo.layers = 1u;

			const VkImageView* pImageView = pCreateInfo->surface->imageView.pBegin;

			const VkFramebuffer* const pFramebufferEnd = _framebuffer.pEnd;
			for (VkFramebuffer* pFramebuffer{ _framebuffer.pBegin }; pFramebuffer != pFramebufferEnd && result == VK_SUCCESS;) {
				createInfo.pAttachments = pImageView++;

				result = vkCreateFramebuffer(device, &createInfo, nullptr, pFramebuffer++);
			}
		}

		if (result != VK_SUCCESS)
			break;

		RenderPass const pass = new(std::nothrow) RenderPass_T;

		if (!pass)
			break;

		pass->renderArea = { { 0u, 0u }, extent };
		pass->framebuffer = _framebuffer;

		pass->pipeline = _pipeline;
		pass->pipelineLayout = _pipelineLayout;
		pass->cameraSetLayout = _cameraSetLayout;
		pass->objectSetLayout = _objectSetLayout;
		pass->renderPass = _renderPass;
		pass->clearValue = _clearValue;

		pass->device = device;

		*pRenderPass = pass;

		return 0;

	} while (false);

	if (_framebuffer) {
		const VkFramebuffer* const pFramebufferEnd = _framebuffer.pEnd;
		for (const VkFramebuffer* pFramebuffer{ _framebuffer.pBegin }; pFramebuffer != pFramebufferEnd && *pFramebuffer; ++pFramebuffer)
			vkDestroyFramebuffer(device, *pFramebuffer, nullptr);

		mem::free_range(_framebuffer);
	}

	if (_pipeline)
		vkDestroyPipeline(device, _pipeline, nullptr);

	if (_pipelineLayout)
		vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);

	if (_objectSetLayout)
		vkDestroyDescriptorSetLayout(device, _objectSetLayout, nullptr);

	if (_cameraSetLayout)
		vkDestroyDescriptorSetLayout(device, _cameraSetLayout, nullptr);

	if (_renderPass)
		vkDestroyRenderPass(device, _renderPass, nullptr);

	if (_clearValue)
		mem::free_range(_clearValue);

	return -1;
}

void destroyRenderPass(RenderPass const _RenderPass) noexcept {
	const VkDevice device = _RenderPass->device;

	const VkFramebuffer* const pFramebufferEnd = _RenderPass->framebuffer.pEnd;
	for (const VkFramebuffer* pFramebuffer{ _RenderPass->framebuffer.pBegin }; pFramebuffer != pFramebufferEnd && *pFramebuffer; ++pFramebuffer)
		vkDestroyFramebuffer(device, *pFramebuffer, nullptr);

	mem::free_range(_RenderPass->framebuffer);

	vkDestroyPipeline(device, _RenderPass->pipeline, nullptr);
	vkDestroyPipelineLayout(device, _RenderPass->pipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(device, _RenderPass->objectSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device, _RenderPass->cameraSetLayout, nullptr);
	vkDestroyRenderPass(device, _RenderPass->renderPass, nullptr);

	mem::free_range(_RenderPass->clearValue);

	delete _RenderPass;
}

int updateRenderPass(RenderPass const _RenderPass, const RenderPassUpdateInfo* const pUpdateInfo) noexcept {
	const VkDevice device = _RenderPass->device;

	mem::span<VkFramebuffer> _framebuffer;

	do {
		VkResult result;

		_framebuffer = mem::allocate_range<VkFramebuffer>(pUpdateInfo->surface->imageView.size());

		if (!_framebuffer)
			break;

		_framebuffer.assign_default();

		const VkExtent2D extent = pUpdateInfo->surface->extent;

		{
			VkFramebufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.renderPass = _RenderPass->renderPass;
			createInfo.attachmentCount = 1u;
			createInfo.width = extent.width;
			createInfo.height = extent.height;
			createInfo.layers = 1u;

			const VkImageView* pImageView = pUpdateInfo->surface->imageView.pBegin;

			const VkFramebuffer* const pFramebufferEnd = _framebuffer.pEnd;
			for (VkFramebuffer* pFramebuffer{ _framebuffer.pBegin }; pFramebuffer != pFramebufferEnd && result == VK_SUCCESS;) {
				createInfo.pAttachments = pImageView++;

				result = vkCreateFramebuffer(device, &createInfo, nullptr, pFramebuffer++);
			}
		}

		if (result != VK_SUCCESS)
			break;

		const VkFramebuffer* const pFramebufferEnd = _RenderPass->framebuffer.pEnd;
		for (const VkFramebuffer* pFramebuffer{ _RenderPass->framebuffer.pBegin }; pFramebuffer != pFramebufferEnd; ++pFramebuffer)
			vkDestroyFramebuffer(device, *pFramebuffer, nullptr);

		mem::free_range(_RenderPass->framebuffer);

		_RenderPass->renderArea = { { 0u, 0u }, extent };
		_RenderPass->framebuffer = _framebuffer;

		return 0;

	} while (false);

	if (_framebuffer) {
		const VkFramebuffer* const pFramebufferEnd = _framebuffer.pEnd;
		for (const VkFramebuffer* pFramebuffer{ _framebuffer.pBegin }; pFramebuffer != pFramebufferEnd && *pFramebuffer; ++pFramebuffer)
			vkDestroyFramebuffer(device, *pFramebuffer, nullptr);

		mem::free_range(_framebuffer);
	}

	return -1;
}

struct Renderer_T {
	VkDevice device;
	VkQueue queue;

	VkCommandPool commandPool;
	mem::span<VkCommandBuffer> commandBuffer;
	mem::span<VkSemaphore> imageSemaphore;
	mem::span<VkSemaphore> renderSemaphore;
	mem::span<VkFence> frameFence;
	
	uint32_t bufferedFrames;
	uint32_t frame;

	uint32_t image;
	VkPipelineLayout pipelineLayout;
};

int createRenderer(Emulator const _Emulator, const RendererCreateInfo* const pCreateInfo, Renderer* const pRenderer) noexcept {
	const VkDevice device = _Emulator->device;

	VkCommandPool _commandPool = VK_NULL_HANDLE;
	mem::span<VkCommandBuffer> _commandBuffer;
	mem::span<VkSemaphore> _imageSemaphore;
	mem::span<VkSemaphore> _renderSemaphore;
	mem::span<VkFence> _frameFence;

	sizeof(Renderer_T);

	do {
		VkResult result;
		
		{
			VkCommandPoolCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			createInfo.queueFamilyIndex = _Emulator->queueFamily.graphics;

			result = vkCreateCommandPool(device, &createInfo, nullptr, &_commandPool);
		}

		if (result != VK_SUCCESS)
			break;

		_commandBuffer = mem::allocate_range<VkCommandBuffer>((size_t)pCreateInfo->maxRenderProcess);

		if (!_commandBuffer)
			break;

		{
			VkCommandBufferAllocateInfo allocateInfo{};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = _commandPool;
			allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = pCreateInfo->maxRenderProcess;

			result = vkAllocateCommandBuffers(device, &allocateInfo, _commandBuffer);
		}

		if (result != VK_SUCCESS)
			break;

		_imageSemaphore = mem::allocate_range<VkSemaphore>((size_t)pCreateInfo->maxRenderProcess);

		if (!_imageSemaphore)
			break;

		_imageSemaphore.assign_default();

		{
			VkSemaphoreCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;

			const VkSemaphore* const pSemaphoreEnd = _imageSemaphore.pEnd;
			for (VkSemaphore* pSemaphore{ _imageSemaphore.pBegin }; pSemaphore != pSemaphoreEnd && result == VK_SUCCESS; ++pSemaphore)
				result = vkCreateSemaphore(device, &createInfo, nullptr, pSemaphore);
		}		

		if (result != VK_SUCCESS)
			break;

		_renderSemaphore = mem::allocate_range<VkSemaphore>((size_t)pCreateInfo->maxRenderProcess);

		if (!_renderSemaphore)
			break;

		_renderSemaphore.assign_default();

		{
			VkSemaphoreCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;

			const VkSemaphore* const pSemaphoreEnd = _renderSemaphore.pEnd;
			for (VkSemaphore* pSemaphore{ _renderSemaphore.pBegin }; pSemaphore != pSemaphoreEnd && result == VK_SUCCESS; ++pSemaphore)
				result = vkCreateSemaphore(device, &createInfo, nullptr, pSemaphore);
		}

		if (result != VK_SUCCESS)
			break;

		_frameFence = mem::allocate_range<VkFence>((size_t)pCreateInfo->maxRenderProcess);

		if (!_frameFence)
			break;

		{
			VkFenceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			const VkFence* const pFenceEnd = _frameFence.pEnd;
			for (VkFence* pFence{ _frameFence.pBegin }; pFence != pFenceEnd && result == VK_SUCCESS; ++pFence)
				result = vkCreateFence(device, &createInfo, nullptr, pFence);
		}

		if (result != VK_SUCCESS)
			break;

		Renderer const renderer = new(std::nothrow) Renderer_T;

		if (!renderer)
			break;
		
		renderer->pipelineLayout = VK_NULL_HANDLE;
		renderer->image = UINT32_MAX;
		
		renderer->frame = 0;
		renderer->bufferedFrames = pCreateInfo->maxRenderProcess;

		renderer->frameFence = _frameFence;
		renderer->renderSemaphore = _renderSemaphore;
		renderer->imageSemaphore = _imageSemaphore;
		renderer->commandBuffer = _commandBuffer;
		renderer->commandPool = _commandPool;

		renderer->queue = _Emulator->queue.graphics;
		renderer->device = device;

		*pRenderer = renderer;

		return 0;

	} while (false);

	if (_frameFence) {
		const VkFence* const pFenceEnd = _frameFence.pEnd;
		for (const VkFence* pFence{ _frameFence.pBegin }; pFence != pFenceEnd && *pFence; ++pFence)
			vkDestroyFence(device, *pFence, nullptr);

		mem::free_range(_frameFence);
	}

	if (_renderSemaphore) {
		const VkSemaphore* const pSemaphoreEnd = _renderSemaphore.pEnd;
		for (const VkSemaphore* pSemaphore{ _renderSemaphore.pBegin }; pSemaphore != pSemaphoreEnd && *pSemaphore; ++pSemaphore)
			vkDestroySemaphore(device, *pSemaphore, nullptr);

		mem::free_range(_renderSemaphore);
	}

	if (_imageSemaphore) {
		const VkSemaphore* const pSemaphoreEnd = _imageSemaphore.pEnd;
		for (const VkSemaphore* pSemaphore{ _imageSemaphore.pEnd }; pSemaphore != pSemaphoreEnd && *pSemaphore; ++pSemaphore)
			vkDestroySemaphore(device, *pSemaphore, nullptr);

		mem::free_range(_imageSemaphore);
	}

	if (_commandBuffer)
		mem::free_range(_commandBuffer);

	if (_commandPool)
		vkDestroyCommandPool(device, _commandPool, nullptr);

	return -1;
}

void destroyRenderer(Renderer const _Renderer) noexcept {
	const VkDevice device = _Renderer->device;

	const VkFence* const pFenceEnd = _Renderer->frameFence.pEnd;
	for (const VkFence* pFence{ _Renderer->frameFence.pBegin }; pFence != pFenceEnd; ++pFence)
		vkDestroyFence(device, *pFence, nullptr);

	mem::free_range(_Renderer->frameFence);

	const VkSemaphore* const pRenderSemaphoreEnd = _Renderer->renderSemaphore.pEnd;
	for (const VkSemaphore* pRenderSemaphore{ _Renderer->renderSemaphore.pBegin }; pRenderSemaphore != pRenderSemaphoreEnd; ++pRenderSemaphore)
		vkDestroySemaphore(device, *pRenderSemaphore, nullptr);

	mem::free_range(_Renderer->renderSemaphore);

	const VkSemaphore* const pImageSemaphoreEnd = _Renderer->imageSemaphore.pEnd;
	for (const VkSemaphore* pImageSemaphore{ _Renderer->imageSemaphore.pBegin }; pImageSemaphore != pImageSemaphoreEnd; ++pImageSemaphore)
		vkDestroySemaphore(device, *pImageSemaphore, nullptr);

	mem::free_range(_Renderer->imageSemaphore);

	mem::free_range(_Renderer->commandBuffer);

	vkDestroyCommandPool(device, _Renderer->commandPool, nullptr);

	delete _Renderer;
}

int waitRenderer(Renderer const _Renderer) noexcept {
	const VkDevice device = _Renderer->device;

	do {
		VkResult result;

		result = vkWaitForFences(device, static_cast<uint32_t>(_Renderer->frameFence.size()), _Renderer->frameFence, VK_TRUE, UINT64_MAX);

		if (result == VK_TIMEOUT)
			return 1;

		if (result != VK_SUCCESS)
			break;

		return 0;

	} while (false);

	return -1;
}

struct InstanceDataGPU {
	InstanceData external;
};

struct Scene_T {
	VkDevice device;
	VmaAllocator allocator;
	
	mem::span<Model_T> model;
	mem::span<VkBuffer> instance;
	mem::span<VmaAllocation> instanceAllocation;
	mem::span<InstanceDataGPU*> instanceData;
	uint32_t instanceCount;
	uint32_t instanceCapacity;

	VkDescriptorPool descriptorPool;
	mem::span<VkDescriptorSet> descriptorSet;

	mem::span<VkBuffer> indirect;
	mem::span<VmaAllocation> indirectAllocation;
	mem::span<VkDrawIndexedIndirectCommand*> indirectData;
	uint32_t indirectCommandCount;
	uint32_t indirectCommandCapacity;
};

int createScene(Emulator const _Emulator, Renderer const _Renderer, const SceneCreateInfo* const pCreateInfo, mem::stack* pScratch, Scene* const pScene) noexcept {
	const VkDevice device = _Emulator->device;
	const VmaAllocator allocator = _Emulator->allocator;

	mem::span<VkBuffer> _instance;
	mem::span<VmaAllocation> _instanceAllocation;
	mem::span<InstanceDataGPU*> _instanceData;

	VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
	mem::span<VkDescriptorSet> _descriptorSet;

	mem::span<VkBuffer> _indirect;
	mem::span<VmaAllocation> _indirectAllocation;
	mem::span<VkDrawIndexedIndirectCommand*> _indirectData;

	do {
		VkResult result = VK_SUCCESS;

		pScratch->mark();

		const size_t frameCount = _Renderer->commandBuffer.size();

		_instance = mem::allocate_range<VkBuffer>(frameCount);

		if (!_instance)
			break;

		_instance.assign_default();

		_instanceAllocation = mem::allocate_range<VmaAllocation>(frameCount);

		if (!_instanceAllocation)
			break;

		_instanceData = mem::allocate_range<InstanceDataGPU*>(frameCount);

		if (!_instanceData)
			break;

		const size_t instanceBufferSize = sizeof(InstanceDataGPU) * pCreateInfo->instanceCount;

		{
			VkBufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.size = (VkDeviceSize)instanceBufferSize;
			createInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0u;
			createInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo allocationCreateInfo{};
			allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
			allocationCreateInfo.requiredFlags = 0;
			allocationCreateInfo.preferredFlags = 0;
			allocationCreateInfo.memoryTypeBits = 0;
			allocationCreateInfo.pool = VK_NULL_HANDLE;
			allocationCreateInfo.pUserData = nullptr;
			allocationCreateInfo.priority = 0.0f;

			VmaAllocationInfo allocationInfo;

			VkBuffer* pBuffer = _instance.pBegin;
			VmaAllocation* pAllocation = _instanceAllocation.pBegin;
			InstanceDataGPU** pData = _instanceData.pBegin;

			const VkBuffer* const pBufferEnd = _instance.pEnd;
			for (; pBuffer != pBufferEnd && result == VK_SUCCESS;) {
				result = vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, pBuffer++, pAllocation++, &allocationInfo);
				*pData++ = reinterpret_cast<InstanceDataGPU*>(allocationInfo.pMappedData);
			}
		}

		if (result != VK_SUCCESS)
			break;

		{
			VkDescriptorPoolSize poolSize[1]{};
			poolSize[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			poolSize[0].descriptorCount = frameCount;

			VkDescriptorPoolCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.maxSets = frameCount;
			createInfo.poolSizeCount = 1u;
			createInfo.pPoolSizes = poolSize;

			result = vkCreateDescriptorPool(device, &createInfo, nullptr, &_descriptorPool);
		}

		if (result != VK_SUCCESS)
			break;

		_descriptorSet = mem::allocate_range<VkDescriptorSet>((size_t)frameCount);

		if (!_descriptorSet)
			break;

		mem::span<VkDescriptorSetLayout> setLayout = pScratch->alloc<VkDescriptorSetLayout>((size_t)frameCount);

		if (!setLayout)
			break;

		setLayout.assign(pCreateInfo->renderBox->objectSetLayout);

		{
			VkDescriptorSetAllocateInfo allocateInfo{};
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.descriptorPool = _descriptorPool;
			allocateInfo.descriptorSetCount = frameCount;
			allocateInfo.pSetLayouts = setLayout;

			result = vkAllocateDescriptorSets(device, &allocateInfo, _descriptorSet);
		}

		if (result != VK_SUCCESS)
			break;

		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.offset = (VkDeviceSize)0u;
			bufferInfo.range = (VkDeviceSize)instanceBufferSize;

			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.pNext = nullptr;
			write.dstBinding = 0u;
			write.dstArrayElement = 0u;
			write.descriptorCount = 1u;
			write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			write.pImageInfo = nullptr;
			write.pBufferInfo = &bufferInfo;
			write.pTexelBufferView = nullptr;

			const VkBuffer* pBuffer = _instance.pBegin;

			const VkDescriptorSet* const pDescriptorSetEnd = _descriptorSet.pEnd;
			for (const VkDescriptorSet* pDescriptorSet{ _descriptorSet.pBegin }; pDescriptorSet != pDescriptorSetEnd;) {
				bufferInfo.buffer = *pBuffer++;
				write.dstSet = *pDescriptorSet++;

				vkUpdateDescriptorSets(device, 1u, &write, 0u, nullptr);
			}
		}

		_indirect = mem::allocate_range<VkBuffer>(frameCount);

		if (!_indirect)
			break;

		_indirect.assign_default();

		_indirectAllocation = mem::allocate_range<VmaAllocation>(frameCount);

		if (!_indirectAllocation)
			break;
		
		_indirectData = mem::allocate_range<VkDrawIndexedIndirectCommand*>(frameCount);

		if (!_indirectData)
			break;

		const uint32_t maxDrawCount = std::min<uint32_t>(pCreateInfo->instanceCount, std::max<uint32_t>(pCreateInfo->drawCount, static_cast<uint32_t>(pCreateInfo->collection->model.size() + 1u)));

		{
			VkBufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.size = sizeof(VkDrawIndexedIndirectCommand) * maxDrawCount;
			createInfo.usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0u;
			createInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo allocationCreateInfo{};
			allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
			allocationCreateInfo.requiredFlags = 0;
			allocationCreateInfo.preferredFlags = 0;
			allocationCreateInfo.memoryTypeBits = 0;
			allocationCreateInfo.pool = VK_NULL_HANDLE;
			allocationCreateInfo.pUserData = nullptr;
			allocationCreateInfo.priority = 0.0f;

			VmaAllocationInfo allocationInfo;

			VkBuffer* pBuffer = _indirect.pBegin;
			VmaAllocation* pAllocation = _indirectAllocation.pBegin;
			VkDrawIndexedIndirectCommand** pData = _indirectData.pBegin;

			const VkBuffer* const pBufferEnd = _indirect.pEnd;
			for (; pBuffer != pBufferEnd && result == VK_SUCCESS;) {
				result = vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, pBuffer++, pAllocation++, &allocationInfo);
				*pData++ = reinterpret_cast<VkDrawIndexedIndirectCommand*>(allocationInfo.pMappedData);
			}
		}

		if (result != VK_SUCCESS)
			break;

		Scene const scene = new(std::nothrow) Scene_T;

		if (!scene)
			break;

		pScratch->restore();

		scene->instanceCapacity = pCreateInfo->instanceCount;
		scene->indirectData = _indirectData;
		scene->indirectAllocation = _indirectAllocation;
		scene->indirect = _indirect;

		scene->descriptorSet = _descriptorSet;
		scene->descriptorPool = _descriptorPool;

		scene->instanceData = _instanceData;
		scene->instanceAllocation = _instanceAllocation;
		scene->instance = _instance;
		scene->model = pCreateInfo->collection->model;

		scene->allocator = allocator;
		scene->device = device;

		*pScene = scene;

		return 0;
	
	} while (false);

	pScratch->restore();

	if (_indirectData)
		mem::free_range(_indirectData);

	if (_indirectAllocation) {
		const VkBuffer* pBuffer = _indirect.pBegin;
		const VmaAllocation* pAllocation = _indirectAllocation.pBegin;

		const VkBuffer* const pBufferEnd = _indirect.pEnd;
		for (; pBuffer != pBufferEnd && *pBuffer;)
			vmaDestroyBuffer(allocator, *pBuffer++, *pAllocation++);

		mem::free_range(_indirectAllocation);
	}

	if (_indirect)
		mem::free_range(_indirect);

	if (_descriptorSet)
		mem::free_range(_descriptorSet);

	if (_descriptorPool)
		vkDestroyDescriptorPool(device, _descriptorPool, nullptr);

	if (_instanceData)
		mem::free_range(_instanceData);

	if (_instanceAllocation) {
		const VkBuffer* pBuffer = _instance.pBegin;
		const VmaAllocation* pAllocation = _instanceAllocation.pBegin;

		const VkBuffer* const pBufferEnd = _instance.pEnd;
		for(; pBuffer != pBufferEnd && *pBuffer;)
			vmaDestroyBuffer(allocator, *pBuffer++, *pAllocation++);
	
		mem::free_range(_instanceAllocation);
	}

	if (_instance)
		mem::free_range(_instance);

	return -1;
}

void destroyScene(Scene const _Scene) noexcept {
	const VkDevice device = _Scene->device;
	const VmaAllocator allocator = _Scene->allocator;
	
	mem::free_range(_Scene->indirectData);

	{
		const VkBuffer* pBuffer = _Scene->indirect.pBegin;
		const VmaAllocation* pAllocation = _Scene->indirectAllocation.pBegin;

		const VkBuffer* const pBufferEnd = _Scene->indirect.pEnd;
		for (; pBuffer != pBufferEnd;)
			vmaDestroyBuffer(allocator, *pBuffer++, *pAllocation++);
	}

	mem::free_range(_Scene->indirectAllocation);
	mem::free_range(_Scene->indirect);

	mem::free_range(_Scene->descriptorSet);
	vkDestroyDescriptorPool(device, _Scene->descriptorPool, nullptr);

	mem::free_range(_Scene->instanceData);
	
	{
		const VkBuffer* pBuffer = _Scene->instance.pBegin;
		const VmaAllocation* pAllocation = _Scene->instanceAllocation.pBegin;
	
		const VkBuffer* const pBufferEnd = _Scene->instance.pEnd;
		for (; pBuffer != pBufferEnd;)
			vmaDestroyBuffer(allocator, *pBuffer++, *pAllocation++);
	}

	mem::free_range(_Scene->instanceAllocation);
	mem::free_range(_Scene->instance);

	delete _Scene;
}

int pushObjectInstance(Scene const _Scene, const ObjectInstance* const pObject) noexcept {
	const uint32_t instanceCount = pObject->instanceCount;
	
	if (!instanceCount)
		return 0;

	if (instanceCount > (_Scene->instanceCapacity - _Scene->instanceCount) || _Scene->indirectCommandCount >= _Scene->indirectCommandCapacity)
		return 1;

	const uint32_t firstInstance = _Scene->instanceCount;
	
	const InstanceData* const pInstanceDataSrcEnd = pObject->pInstances + pObject->instanceCount;
	
	const InstanceDataGPU* const* const ppInstanceDataGPUEnd = _Scene->instanceData.pEnd;
	for (InstanceDataGPU* const* ppInstanceDataGPU{ _Scene->instanceData.pBegin }; ppInstanceDataGPU != ppInstanceDataGPUEnd;) {
		InstanceDataGPU* pInstanceDataDst = *ppInstanceDataGPU++ + firstInstance;

		for (const InstanceData* pInstanceDataSrc{ pObject->pInstances }; pInstanceDataSrc != pInstanceDataSrcEnd;)
			memcpy((void*)(reinterpret_cast<uint8_t*>(pInstanceDataDst++) + offsetof(InstanceDataGPU, InstanceDataGPU::external)), (void*)pInstanceDataSrc++, sizeof(InstanceData));
	}

	const Model_T* const pModel = &_Scene->model[pObject->model];

	VkDrawIndexedIndirectCommand command{};
	command.indexCount = pModel->indexcount;
	command.instanceCount = pObject->instanceCount;
	command.firstIndex = static_cast<uint32_t>(pModel->firstIndex);
	command.vertexOffset = static_cast<int32_t>(pModel->firstVertex);
	command.firstInstance = firstInstance;

	const uint32_t drawIndex = _Scene->indirectCommandCount;

	const VkDrawIndexedIndirectCommand* const* const ppDrawDataEnd = _Scene->indirectData.pEnd;
	for (VkDrawIndexedIndirectCommand* const* ppDrawData{ _Scene->indirectData.pBegin }; ppDrawData != ppDrawDataEnd;)
		*ppDrawData[drawIndex] = command;

	_Scene->instanceCount += instanceCount;
	++_Scene->indirectCommandCount;

	return 0;
}

struct CameraDataGPU {
	glm::mat4 camera;
	glm::mat4 projection;
};

struct Camera_T {
	VkDevice device;
	VmaAllocator allocator;

	mem::span<VkBuffer> buffer;
	mem::span<VmaAllocation> allocation;
	mem::span<CameraDataGPU*> data;

	VkDescriptorPool descriptorPool;
	mem::span<VkDescriptorSet> descriptorSet;
};

int createCamera(Emulator const _Emulator, Renderer const _Renderer, const CameraCreateInfo* const pCreateInfo, mem::stack* const pScratch, Camera* const pCamera) noexcept {
	const VkDevice device = _Emulator->device;
	const VmaAllocator allocator = _Emulator->allocator;

	mem::span<VkBuffer> _buffer;
	mem::span<VmaAllocation> _allocation;
	mem::span<CameraDataGPU*> _data;

	VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
	mem::span<VkDescriptorSet> _descriptorSet;

	do {
		VkResult result{};

		pScratch->mark();

		const uint32_t frameCount = static_cast<uint32_t>(_Renderer->frameFence.size());

		_buffer = mem::allocate_range<VkBuffer>((size_t)frameCount);

		if (!_buffer)
			break;

		_buffer.assign_default();

		_allocation = mem::allocate_range<VmaAllocation>((size_t)frameCount);

		if (!_allocation)
			break;

		_data = mem::allocate_range<CameraDataGPU*>((size_t)frameCount);

		if (!_data)
			break;

		const size_t size = sizeof(CameraDataGPU) * pCreateInfo->bindings;

		{
			VkBufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.size = (VkDeviceSize)size;
			createInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0u;
			createInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo allocationCreateInfo{};
			allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
			allocationCreateInfo.requiredFlags = 0;
			allocationCreateInfo.preferredFlags = 0;
			allocationCreateInfo.memoryTypeBits = 0;
			allocationCreateInfo.pool = VK_NULL_HANDLE;
			allocationCreateInfo.pUserData = nullptr;
			allocationCreateInfo.priority = 0.0f;

			VmaAllocationInfo allocationInfo;

			CameraDataGPU** ppData = _data.pBegin;
			VmaAllocation* pAllocation = _allocation.pBegin;

			const VkBuffer* const pBufferEnd = _buffer.pEnd;
			for (VkBuffer* pBuffer{_buffer.pBegin}; pBuffer != pBufferEnd && result == VK_SUCCESS;) {
				result = vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, pBuffer++, pAllocation++, &allocationInfo);

				*ppData++ = reinterpret_cast<CameraDataGPU*>(allocationInfo.pMappedData);
			}
		}

		if (result != VK_SUCCESS)
			break;

		{
			VkDescriptorPoolSize poolSize[1]{};
			poolSize[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			poolSize[0].descriptorCount = frameCount;

			VkDescriptorPoolCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.maxSets = frameCount;
			createInfo.poolSizeCount = 1u;
			createInfo.pPoolSizes = poolSize;

			result = vkCreateDescriptorPool(device, &createInfo, nullptr, &_descriptorPool);
		}

		if (result != VK_SUCCESS)
			break;

		_descriptorSet = mem::allocate_range<VkDescriptorSet>((size_t)frameCount);

		if (!_descriptorSet)
			break;		

		mem::span<VkDescriptorSetLayout> setLayout = pScratch->alloc<VkDescriptorSetLayout>(frameCount);

		if (!setLayout)
			break;

		setLayout.assign(pCreateInfo->renderBox->cameraSetLayout);

		{
			VkDescriptorSetAllocateInfo allocateInfo{};
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.descriptorPool = _descriptorPool;
			allocateInfo.descriptorSetCount = frameCount;
			allocateInfo.pSetLayouts = setLayout;

			result = vkAllocateDescriptorSets(device, &allocateInfo, _descriptorSet);
		}

		if (result != VK_SUCCESS)
			break;

		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.offset = (VkDeviceSize)0u;
			bufferInfo.range = (VkDeviceSize)size;

			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.pNext = nullptr;
			write.dstBinding = 0u;
			write.dstArrayElement = 0u;
			write.descriptorCount = 1u;
			write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			write.pImageInfo = nullptr;
			write.pBufferInfo = &bufferInfo;
			write.pTexelBufferView = nullptr;

			const VkBuffer* pBuffer = _buffer.pBegin;

			const VkDescriptorSet* const pDescriptorSetEnd = _descriptorSet.pEnd;
			for (const VkDescriptorSet* pDescriptorSet{ _descriptorSet.pBegin }; pDescriptorSet != pDescriptorSetEnd;) {
				bufferInfo.buffer = *pBuffer++;
				write.dstSet = *pDescriptorSet++;

				vkUpdateDescriptorSets(device, 1u, &write, 0, nullptr);
			}
		}

		Camera const camera = new(std::nothrow) Camera_T;

		if (!camera)
			break;

		pScratch->restore();

		camera->descriptorSet = _descriptorSet;
		camera->descriptorPool = _descriptorPool;

		camera->data = _data;
		camera->allocation = _allocation;
		camera->buffer = _buffer;
		
		camera->allocator = allocator;
		camera->device = device;

		return 0;

	} while (false);

	pScratch->restore();

	if (_descriptorSet)
		mem::free_range(_descriptorSet);

	if (_descriptorPool)
		vkDestroyDescriptorPool(device, _descriptorPool, nullptr);

	if (_data)
		mem::free_range(_data);

	if (_allocation) {
		const VmaAllocation* pAllocation = _allocation.pBegin;

		const VkBuffer* const pBufferEnd = _buffer.pEnd;
		for (const VkBuffer* pBuffer{ _buffer.pBegin }; pBuffer != pBufferEnd && *pBuffer;)
			vmaDestroyBuffer(allocator, *pBuffer++, *pAllocation++);

		mem::free_range(_allocation);
	}

	if (_buffer)
		mem::free_range(_buffer);

	return -1;
}

void destroyCamera(Camera const _Camera) noexcept {
	const VkDevice device = _Camera->device;
	const VmaAllocator allocator = _Camera->allocator;

	mem::free_range(_Camera->descriptorSet);
	vkDestroyDescriptorPool(device, _Camera->descriptorPool, nullptr);

	mem::free_range(_Camera->data);

	const VmaAllocation* pAllocation = _Camera->allocation.pBegin;
	
	const VkBuffer* const pBufferEnd = _Camera->buffer.pEnd;
	for (const VkBuffer* pBuffer{ _Camera->buffer.pBegin }; pBuffer != pBufferEnd && *pBuffer;)
		vmaDestroyBuffer(allocator, *pBuffer++, *pAllocation++);

	mem::free_range(_Camera->allocation);
	mem::free_range(_Camera->buffer);

	delete _Camera;
}

int beginFrame(Renderer const _Renderer, Surface const _Surface) noexcept {
	const VkDevice device = _Renderer->device;
	
	VkResult result;
	
	const uint32_t frame = _Renderer->frame;

	const VkSwapchainKHR swapchain = _Surface->swapchain;
	const VkSemaphore imageAvailable = _Renderer->imageSemaphore[frame];

	const VkFence frameFence = _Renderer->frameFence[frame];

	result = vkWaitForFences(device, 1u, &frameFence, VK_TRUE, UINT64_MAX);

	if (result != VK_SUCCESS)
		return -1;

	uint32_t imageIndex;
	result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
		return 1;

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		return -1;

	VkFence imageFence = _Surface->fence[imageIndex];

	if (!imageFence)
		imageFence = frameFence;

	result = vkWaitForFences(device, 1u, &imageFence, VK_TRUE, UINT64_MAX);

	if (result != VK_SUCCESS)
		return -1;

	const VkCommandBuffer commandBuffer = _Renderer->commandBuffer[frame];

	result = vkResetCommandBuffer(commandBuffer, 0);

	if (result != VK_SUCCESS)
		return -1;

	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pNext = nullptr;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
	}

	if (result != VK_SUCCESS)
		return -1;

	_Renderer->image = imageIndex;

	return 0;
}

void beginRenderPass(Renderer const _Renderer, RenderPass const _RenderPass, Camera const _Camera) noexcept {
	const VkCommandBuffer commandBuffer = _Renderer->commandBuffer[_Renderer->frame];

	{
		VkRenderPassBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.pNext = nullptr;
		beginInfo.renderPass = _RenderPass->renderPass;
		beginInfo.framebuffer = _RenderPass->framebuffer[_Renderer->image];
		beginInfo.renderArea = _RenderPass->renderArea;
		beginInfo.clearValueCount = static_cast<uint32_t>(_RenderPass->clearValue.size());
		beginInfo.pClearValues = _RenderPass->clearValue;

		vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _RenderPass->pipeline);

	{
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = _RenderPass->renderArea.extent.width;
		viewport.height = _RenderPass->renderArea.extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}
	
	vkCmdSetScissor(commandBuffer, 0, 1, &_RenderPass->renderArea);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _RenderPass->pipelineLayout, 0u, 1u, &_Camera->descriptorSet[_Renderer->frame], 0u, nullptr);

	_Renderer->pipelineLayout = _RenderPass->pipelineLayout;
}

void setActiveCamera(Renderer _Renderer, uint32_t camera) noexcept {
	vkCmdPushConstants(_Renderer->commandBuffer[_Renderer->frame], _Renderer->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0u, sizeof(uint32_t), &camera);
}

void render(Renderer const _Renderer, Collection const _Collection, Scene const _Scene) noexcept {
	const uint32_t frame = _Renderer->frame;
	const VkCommandBuffer commandBuffer = _Renderer->commandBuffer[frame];

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_Collection->vertex, 0);
	vkCmdBindIndexBuffer(commandBuffer, _Collection->index, 0u, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _Renderer->pipelineLayout, 0u, 1u, &_Scene->descriptorSet[frame], 0u, nullptr);
			
	vkCmdDrawIndexedIndirect(commandBuffer, _Scene->indirect[frame], 0, _Scene->indirectCommandCount, sizeof(VkDrawIndexedIndirectCommand));
}

void endPass(Renderer const _Renderer) noexcept {
	vkCmdEndRenderPass(_Renderer->commandBuffer[_Renderer->frame]);
}

int endFrame(Renderer const _Renderer, Surface const _Surface) noexcept {
	const uint32_t frame = _Renderer->frame;
	const VkCommandBuffer commandBuffer = _Renderer->commandBuffer[frame];

	VkResult result;

	result = vkEndCommandBuffer(commandBuffer);

	if (result != VK_SUCCESS)
		return -1;

	const VkFence fence = _Renderer->frameFence[frame];

	result = vkResetFences(_Renderer->device, 1u, &fence);

	const VkSemaphore imageAvailable = _Renderer->imageSemaphore[frame];
	const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	const VkSemaphore renderComplete = _Renderer->renderSemaphore[frame];

	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 1u;
		submitInfo.pWaitSemaphores = &imageAvailable;
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1u;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1u;
		submitInfo.pSignalSemaphores = &renderComplete;

		result = vkQueueSubmit(_Renderer->queue, 1u, &submitInfo, fence);
	}

	if (result != VK_SUCCESS)
		return -1;

	_Surface->fence[_Renderer->image] = fence;

	return 0;
}

int presentFrame(Renderer const _Renderer, Surface _Surface) noexcept {
	VkResult result;
	
	const VkSemaphore semaphore = _Renderer->renderSemaphore[_Renderer->frame];

	{
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1u;
		presentInfo.pWaitSemaphores = &semaphore;
		presentInfo.swapchainCount = 1u;
		presentInfo.pSwapchains = &_Surface->swapchain;
		presentInfo.pResults = nullptr;
		presentInfo.pImageIndices = &_Renderer->image;

		result = vkQueuePresentKHR(_Surface->queue, &presentInfo);
	}

	_Renderer->frame++;

	if (_Renderer->frame > _Renderer->bufferedFrames)
		_Renderer->frame = 0u;

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		return 1;

	if (result != VK_SUCCESS)
		return -1;

	return 0;
}
