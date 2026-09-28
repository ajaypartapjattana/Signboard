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

#define NULLABLE

constexpr uint32_t INVALID_QUEUE_FAMILY = UINT32_MAX;

struct PhysicalDeviceInfo {
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceMemoryProperties memory;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDevice handle;
};

struct VulkanContext_T {
	VkInstance instance;
	PhysicalDeviceInfo* deviceInfo;
};

int createVulkanContext(mem_stack* const pScratch, VulkanContext* const pContext) noexcept {
	VkInstance _instance = VK_NULL_HANDLE;

	PhysicalDeviceInfo* _deviceInfo = nullptr;
	
	do {
		VkResult result;
		
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
			return -1;

		pScratch->frame();

		uint32_t physicalDeviceCount;

		result = vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr);

		if (result != VK_SUCCESS)
			break;

		_deviceInfo = mem_allocateSizeRange<PhysicalDeviceInfo>((size_t)physicalDeviceCount);

		if (!_deviceInfo)
			break;

		mem_span<VkPhysicalDevice> physicalDevice;
		mem_stackAllocateSpan<VkPhysicalDevice>(pScratch, &physicalDevice, (size_t)physicalDeviceCount);

		if (!physicalDevice.data)
			break;

		result = vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, physicalDevice.data);
		
		if (result != VK_SUCCESS)
			break;

		PhysicalDeviceInfo* pDeviceInfo = _deviceInfo;

		const VkPhysicalDevice* const pPhysicalDeviceEnd = physicalDevice.end();
		for (const VkPhysicalDevice* pPhysicalDevice{ physicalDevice.data }; pPhysicalDevice != pPhysicalDeviceEnd;) {
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
		mem_freeSizeRange<PhysicalDeviceInfo>(_deviceInfo);

	if (_instance)
		vkDestroyInstance(_instance, nullptr);

	return -1;
}

void destroyVulkanContext(VulkanContext const _Context) noexcept {
	mem_freeSizeRange<PhysicalDeviceInfo>(_Context->deviceInfo);
	vkDestroyInstance(_Context->instance, nullptr);

	delete _Context;
}

void getPhysicalDeviceCount(VulkanContext const _Context, uint32_t* const pCount) noexcept {
	*pCount = static_cast<uint32_t>(mem_getSizeAllocationSize(_Context->deviceInfo));
}

void queryPerformaceOptimalDevice(VulkanContext const _Context, const uint32_t minIndex, int* const pIndex) noexcept {
	const PhysicalDeviceInfo* const pDeviceEnd = mem_getSizeAllcoationEnd(_Context->deviceInfo);
	for (const PhysicalDeviceInfo* pDevice{ _Context->deviceInfo + minIndex }; pDevice != pDeviceEnd; ++pDevice) {
		if (pDevice->properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			continue;

		*pIndex = static_cast<uint32_t>(pDevice - _Context->deviceInfo);
		return;
	}

	*pIndex = -1;
}

void queryBatteryOptimalDevice(VulkanContext const _Context, const uint32_t minIndex, int* const pIndex) noexcept {
	const PhysicalDeviceInfo* const pDeviceEnd = mem_getSizeAllcoationEnd(_Context->deviceInfo);
	for (const PhysicalDeviceInfo* pDevice{ _Context->deviceInfo + minIndex }; pDevice != pDeviceEnd; ++pDevice) {
		if (pDevice->properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			continue;

		*pIndex = static_cast<uint32_t>(pDevice - _Context->deviceInfo);
		return;
	}

	*pIndex = -1;
}

void getPhysicalDeviceName(VulkanContext const _Context, const uint32_t index, const char** pName) noexcept {
	const PhysicalDeviceInfo* const pDevice = _Context->deviceInfo + index;

	*pName = pDevice->properties.deviceName;
}

void enumeratePhysicalDeviceName(VulkanContext const _Context, const uint32_t minIndex, const uint32_t count, const char** const pDeviceNames) noexcept {
	const PhysicalDeviceInfo* pDeviceInfo = _Context->deviceInfo + minIndex;
	
	const char** const pNameEnd = pDeviceNames + count;
	for(const char** pName{ pDeviceNames }; pName != pNameEnd; ++pName)
		*pName = pDeviceInfo++->properties.deviceName;
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

struct StagingState {
	mem_span<uint8_t> span;
	uint8_t* pHead;
};

struct StageRegion {
	VkDeviceSize offset;
	VkDeviceSize size;
};

struct SubmissionResource {
	uint32_t capacity;
	uint32_t index;
	VkCommandPool commandPool;
	VkCommandBuffer* commandBuffer;
	VkSemaphore* semaphore;
	VkFence* fence;

	inline VkCommandBuffer thisCommandBuffer() const noexcept {
		return commandBuffer[index];
	}

	inline VkSemaphore thisSemaphore() const noexcept {
		return semaphore[index];
	}

	inline VkFence thisFence() const noexcept {
		return fence[index];
	}
};

static int createSubmissionResource(const VkDevice device, const uint32_t familyIndex, const uint32_t capacity, SubmissionResource* const pResource) noexcept {
	{
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		createInfo.queueFamilyIndex = familyIndex;

		if (vkCreateCommandPool(device, &createInfo, nullptr, &pResource->commandPool) != VK_SUCCESS)
			return -1;
	}

	pResource->commandBuffer = mem_allocateRange<VkCommandBuffer>(capacity);

	if (!pResource->commandBuffer)
		goto failure_0;
		
	{
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.pNext = nullptr;
		allocateInfo.commandPool = pResource->commandPool;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = capacity;
			
		if (vkAllocateCommandBuffers(device, &allocateInfo, pResource->commandBuffer) != VK_SUCCESS)
			goto failure_1;
	}

	pResource->semaphore = mem_allocateRange<VkSemaphore>(capacity);

	if (!pResource->semaphore)
		goto failure_1;

	mem_nullifyRange<VkSemaphore>(pResource->semaphore, capacity);
	
	{
		VkSemaphoreCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;

		const VkSemaphore* const pSemaphoreEnd = pResource->semaphore + capacity;
		for(VkSemaphore* pSemaphore{ pResource->semaphore }; pSemaphore != pSemaphoreEnd; ++pSemaphore)
			if (vkCreateSemaphore(device, &createInfo, nullptr, pSemaphore) != VK_SUCCESS)
				goto failure_2;
	}

	pResource->fence = mem_allocateRange<VkFence>(capacity);

	if (!pResource->fence)
		goto failure_2;

	mem_nullifyRange<VkFence>(pResource->fence, capacity);

	{
		VkFenceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		const VkFence* const pFenceEnd = pResource->fence + capacity;
		for (VkFence* pFence{ pResource->fence }; pFence != pFenceEnd; ++pFence)
			if (vkCreateFence(device, &createInfo, nullptr, pFence) != VK_SUCCESS)
				goto failure_3;
	}

	pResource->capacity = capacity;
	pResource->index = 0u;

	return 0;

failure_3:
	{
		const VkFence* const pFenceEnd = pResource->fence + capacity;
		for (VkFence* pFence{ pResource->fence }; pFence != pFenceEnd; ++pFence)
			vkDestroyFence(device, *pFence, nullptr);
	}

	mem_freeRange<VkFence>(pResource->fence);

failure_2:
	{
		const VkSemaphore* const pSemaphoreEnd = pResource->semaphore + capacity;
		for (const VkSemaphore* pSemaphore{ pResource->semaphore }; pSemaphore != pSemaphoreEnd && *pSemaphore; ++pSemaphore)
			vkDestroySemaphore(device, *pSemaphore, nullptr);
	}

	mem_freeRange<VkSemaphore>(pResource->semaphore);

failure_1:
	mem_freeRange<VkCommandBuffer>(pResource->commandBuffer);

failure_0:
	vkDestroyCommandPool(device, pResource->commandPool, nullptr);

	return -1;
}

static void destroySubmissionResource(const VkDevice device, const SubmissionResource* const pResource) noexcept {
	const VkFence* const pFenceEnd = pResource->fence + pResource->capacity;
	for (const VkFence* pFence{ pResource->fence }; pFence != pFenceEnd && *pFence; ++pFence)
		vkDestroyFence(device, *pFence, nullptr);

	mem_freeRange<VkFence>(pResource->fence);

	const VkSemaphore* const pSemaphoreEnd = pResource->semaphore + pResource->capacity;
	for (const VkSemaphore* pSemaphore{ pResource->semaphore }; pSemaphore != pSemaphoreEnd && *pSemaphore; ++pSemaphore)
		vkDestroySemaphore(device, *pSemaphore, nullptr);

	mem_freeRange<VkSemaphore>(pResource->semaphore);

	mem_freeRange<VkCommandBuffer>(pResource->commandBuffer);

	vkDestroyCommandPool(device, pResource->commandPool, nullptr);
}

struct BasePass {
	VkRenderPass renderPass;
	VkClearValue clearValue[2];
	VkSampler sampler;
	VkDescriptorSetLayout cameraSetLayout;
	VkDescriptorSetLayout objectSetLayout;
	VkDescriptorSetLayout textureSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
};

static int createBasePassResources(const VkDevice device, const VkFormat colorFormat, const VkFormat depthFormat, mem_stack* const pScratch, BasePass* const pPass) noexcept {
	{
		VkAttachmentDescription attachment[2]{};
		attachment[0].format = colorFormat;
		attachment[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachment[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		attachment[1].format = depthFormat;
		attachment[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachment[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference reference[2]{};
		reference[0].attachment = 0;
		reference[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		reference[1].attachment = 1;
		reference[1].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.flags = 0;
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.inputAttachmentCount = 0;
		subpass.pInputAttachments = nullptr;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &reference[0];
		subpass.pResolveAttachments = nullptr;
		subpass.pDepthStencilAttachment = &reference[1];
		subpass.preserveAttachmentCount = 0;
		subpass.pPreserveAttachments = nullptr;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = VK_ACCESS_NONE;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependency.dependencyFlags = 0;

		VkRenderPassCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.attachmentCount = 2u;
		createInfo.pAttachments = attachment;
		createInfo.subpassCount = 1u;
		createInfo.pSubpasses = &subpass;
		createInfo.dependencyCount = 1u;
		createInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &createInfo, nullptr, &pPass->renderPass) != VK_SUCCESS)
			return -1;
	}

	pPass->clearValue[0] = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
	pPass->clearValue[1] = { 1.0f, 0 };

	{
		VkSamplerCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.magFilter = VK_FILTER_NEAREST;
		createInfo.minFilter = VK_FILTER_NEAREST;
		createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.mipLodBias = 0.0f;
		createInfo.anisotropyEnable = VK_FALSE;
		createInfo.maxAnisotropy = 1.0f;
		createInfo.compareEnable = VK_FALSE;
		createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		createInfo.minLod = 0.0f;
		createInfo.maxLod = 0.0f;
		createInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		createInfo.unnormalizedCoordinates = VK_FALSE;

		if (vkCreateSampler(device, &createInfo, nullptr, &pPass->sampler) != VK_SUCCESS)
			goto failure_1;
	}

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

		if (vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &pPass->cameraSetLayout) != VK_SUCCESS)
			goto failure_2;
	}

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

		if (vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &pPass->objectSetLayout) != VK_SUCCESS)
			goto failure_3;
	}

	{
		VkDescriptorSetLayoutBinding binding[1]{};
		binding[0].binding = 0;
		binding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding[0].descriptorCount = 1u;
		binding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		binding[0].pImmutableSamplers = &pPass->sampler;

		VkDescriptorSetLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.bindingCount = 1u;
		createInfo.pBindings = binding;

		if (vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &pPass->textureSetLayout) != VK_SUCCESS)
			goto failure_4;
	}

	{
		const VkDescriptorSetLayout setLayouts[3] = { pPass->cameraSetLayout, pPass->objectSetLayout, pPass->textureSetLayout };

		VkPushConstantRange pushRange{};
		pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushRange.size = sizeof(uint32_t);
		pushRange.offset = 0u;

		VkPipelineLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.setLayoutCount = 3u;
		createInfo.pSetLayouts = setLayouts;
		createInfo.pushConstantRangeCount = 1u;
		createInfo.pPushConstantRanges = &pushRange;

		if (vkCreatePipelineLayout(device, &createInfo, nullptr, &pPass->pipelineLayout) != VK_SUCCESS)
			goto failure_5;
	}

	{
		VkShaderModule vertex;
	
		{
			const char* const vertPath = "shaders/base.vert.spv";

			size_t size;

			if (io::getBinarySize(vertPath, &size))
				goto failure_6;

			pScratch->frame();

			uint8_t* bin = mem_stackAllocateRange<uint8_t>(pScratch, size);

			if (!bin) {
				pScratch->restore();
				goto failure_6;
			}

			if (io::loadBinary(vertPath, size, bin)) {
				pScratch->restore();
				goto failure_6;
			}

			{
				VkShaderModuleCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				createInfo.codeSize = size;
				createInfo.pCode = reinterpret_cast<const uint32_t*>(bin);

				if (vkCreateShaderModule(device, &createInfo, nullptr, &vertex) != VK_SUCCESS) {
					pScratch->restore();
					goto failure_6;
				}
			}

			pScratch->restore();
		}
		
		VkShaderModule fragment;

		{
			const char* const fragPath = "shaders/base.frag.spv";

			size_t size;

			if (io::getBinarySize(fragPath, &size)) {
				vkDestroyShaderModule(device, vertex, nullptr);
				goto failure_6;
			}

			pScratch->frame();

			uint8_t* bin = mem_stackAllocateRange<uint8_t>(pScratch, size);

			if (!bin) {
				pScratch->restore();
				vkDestroyShaderModule(device, vertex, nullptr);
				goto failure_6;
			}

			if (io::loadBinary(fragPath, size, bin)) {
				pScratch->restore();
				vkDestroyShaderModule(device, vertex, nullptr);
				goto failure_6;
			}

			{
				VkShaderModuleCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				createInfo.codeSize = size;
				createInfo.pCode = reinterpret_cast<const uint32_t*>(bin);

				if (vkCreateShaderModule(device, &createInfo, nullptr, &fragment) != VK_SUCCESS) {
					pScratch->restore();
					vkDestroyShaderModule(device, vertex, nullptr);
					goto failure_6;
				}
			}

			pScratch->restore();
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
		attribute[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute[0].offset = offsetof(Vertex, Vertex::pos);

		attribute[1].binding = 0;
		attribute[1].location = 1;
		attribute[1].format = VK_FORMAT_R32G32_SFLOAT;
		attribute[1].offset = offsetof(Vertex, Vertex::uv);

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
		rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
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
		depthStencilState.depthTestEnable = VK_TRUE;
		depthStencilState.depthWriteEnable = VK_TRUE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
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
		createInfo.layout = pPass->pipelineLayout;
		createInfo.renderPass = pPass->renderPass;
		createInfo.subpass = 0;
		createInfo.basePipelineHandle = VK_NULL_HANDLE;
		createInfo.basePipelineIndex = 0;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1u, &createInfo, nullptr, &pPass->pipeline)) {
			vkDestroyShaderModule(device, vertex, nullptr);
			vkDestroyShaderModule(device, fragment, nullptr);
			goto failure_6;
		}

		vkDestroyShaderModule(device, vertex, nullptr);
		vkDestroyShaderModule(device, fragment, nullptr);
	}

	return 0;

failure_6:
	vkDestroyPipelineLayout(device, pPass->pipelineLayout, nullptr);

failure_5:
	vkDestroyDescriptorSetLayout(device, pPass->textureSetLayout, nullptr);

failure_4:
	vkDestroyDescriptorSetLayout(device, pPass->objectSetLayout, nullptr);

failure_3:
	vkDestroyDescriptorSetLayout(device, pPass->cameraSetLayout, nullptr);

failure_2:
	vkDestroySampler(device, pPass->sampler, nullptr);

failure_1:
	vkDestroyRenderPass(device, pPass->renderPass, nullptr);

	return -1;
}

static void destroyBasePassResources(const VkDevice device, const BasePass* const pPass) noexcept {
	vkDestroyPipeline(device, pPass->pipeline, nullptr);
	vkDestroyPipelineLayout(device, pPass->pipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(device, pPass->textureSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device, pPass->objectSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device, pPass->cameraSetLayout, nullptr);
	vkDestroySampler(device, pPass->sampler, nullptr);
	vkDestroyRenderPass(device, pPass->renderPass, nullptr);
}

struct __Renderer_T {
	VkPhysicalDevice physicalDevice;
	VkSurfaceKHR surface;

	VkDevice device;
	VmaAllocator allocator;
	QueueFamilyIndices queueFamily;
	DeviceQueues queue;
	
	VkSurfaceFormatKHR format;
	VkPresentModeKHR presentMode;
	VkFormat depthFormat;

	VkSwapchainKHR swapchain;
	VkExtent2D extent;
	uint32_t layers;
	uint32_t imageCount;
	VkImage* image;
	VkImage depthImage;
	VmaAllocation depthImageAllocation;
	VkImageView* imageView;
	VkImageView depthImageView;
	VkFence* imageFence;
	
	BasePass pass;
	VkFramebuffer* framebuffer;

	uint32_t acquire;
	VkSemaphore* imageAvailable;
	SubmissionResource graphics;
	
	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;
	mem_span<uint8_t> stage;
	uint8_t** region;
	SubmissionResource transfer;
};

int createRenderDevice(VulkanContext const context, const EmulatorCreateInfo* const pCreateInfo, mem_stack* const pScratch, __Renderer* const pRenderer) noexcept {
	__Renderer const renderer = new(std::nothrow) __Renderer_T;

	if (!renderer)
		return -1;

	const PhysicalDeviceInfo* const pPhysicalDeviceInfo = context->deviceInfo + pCreateInfo->physicalDevice;
	renderer->physicalDevice = pPhysicalDeviceInfo->handle;

#if defined(WINDOW_WIN32)
	{
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.connection = reinterpret_cast<HINSTANCE>(pCreateInfo->windowContext);
		createInfo.window = static_cast<HWND>(pCreateInfo->windowHandle);

		result = vkCreateWin32SurfaceKHR(context->instance, &createInfo, nullptr, &renderer->surface);
	}
#elif defined(WINDOW_XCB)
	{
		VkXcbSurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.connection = reinterpret_cast<xcb_connection_t*>(pCreateInfo->windowContext);
		createInfo.window = static_cast<xcb_window_t>(pCreateInfo->windowHandle);

		if (vkCreateXcbSurfaceKHR(context->instance, &createInfo, nullptr, &renderer->surface) != VK_SUCCESS)
			goto failure_0;
	}
#elif defined(WINDOW_WAYLAND)
  	{
		VkWaylandSurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.display = reinterpret_cast<wl_display*>(pCreateInfo->windowContext);
		createInfo.surface = reinterpret_cast<wl_surface*>(pCreateInfo->windowHandle);

		result = vkCreateWaylandSurfaceKHR(context->instance, &createInfo, nullptr, &renderer->surface);
	}
#elif defined(WINDOW_X11)
  	{
		VkXlibSurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.dpy = reinterpret_cast<Display*>(pCreateInfo->windowContext);
		createInfo.window = static_cast<::Window>(pCreateInfo->windowHandle);

		result = vkCreateXlibSurfaceKHR(context->instance, &createInfo, nullptr, &renderer->surface);
	}
#endif

	{
		pScratch->frame();

		mem_smallset<uint32_t> uniqueQueueFamily;

		mem_stackAllocateSmallSet<uint32_t>(pScratch, &uniqueQueueFamily, 3u);

		if (!uniqueQueueFamily.data) {
			pScratch->restore();
			goto failure_1;
		}

		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(renderer->physicalDevice, &queueFamilyCount, nullptr);
		
		VkQueueFamilyProperties* const queueFamilies = mem_stackAllocateRange<VkQueueFamilyProperties>(pScratch, (size_t)queueFamilyCount);

		if (!queueFamilies) {
			pScratch->restore();
			goto failure_1;
		}

		vkGetPhysicalDeviceQueueFamilyProperties(renderer->physicalDevice, &queueFamilyCount, queueFamilies);

		
		uint32_t* familyRole = mem_stackAllocateRange<uint32_t>(pScratch, (size_t)queueFamilyCount);

		if (!familyRole) {
			pScratch->restore();
			goto failure_1;
		}

		mem_nullifyRange(familyRole, (size_t)queueFamilyCount);

		{
			constexpr VkQueueFlags RELEVANT = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;

			const VkQueueFamilyProperties* pFamilyProperties = queueFamilies;

			const uint32_t* const pRoleEnd = familyRole + queueFamilyCount;
			for (uint32_t* pRole{ familyRole }; pRole != pRoleEnd;)
				*pRole++ = intrin_popCount32(pFamilyProperties++->queueFlags & RELEVANT);
		}
			
		uint32_t min = UINT32_MAX;

		for (uint32_t i = 0; i < queueFamilyCount; ++i) {
			if((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
				continue;

			if (familyRole[i] >= min)
				continue;

			min = familyRole[i];
			renderer->queueFamily.graphics = i;
			uniqueQueueFamily.push(i);
		}

		min = UINT32_MAX;

		for (uint32_t i = 0; i < queueFamilyCount; ++i) {
			if ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) == 0)
				continue;

			if (familyRole[i] >= min)
				continue;

			min = familyRole[i];
			renderer->queueFamily.transfer = i;
			uniqueQueueFamily.push(i);
		}

		VkBool32 presentSupport = VK_FALSE;

		for (uint32_t i = 0; i < queueFamilyCount; ++i) {
			if (vkGetPhysicalDeviceSurfaceSupportKHR(renderer->physicalDevice, i, renderer->surface, &presentSupport) != VK_SUCCESS) {
				pScratch->restore();
				goto failure_1;
			}
			
			if (!presentSupport)
				continue;
					
			renderer->queueFamily.present = i;
			uniqueQueueFamily.push(i);

			break;
		}

		if (uniqueQueueFamily.empty()) {
			pScratch->restore();
			goto failure_1;
		}

		const uint32_t queueInfoCount = static_cast<uint32_t>(uniqueQueueFamily.size());
		VkDeviceQueueCreateInfo* const queueInfo = mem_stackAllocateRange<VkDeviceQueueCreateInfo>(pScratch, queueInfoCount);

		constexpr float queuePriority = 1.0f;

		const uint32_t* pFamily = uniqueQueueFamily.data;

		const VkDeviceQueueCreateInfo* pQueueInfoEnd = queueInfo + queueInfoCount;
		for (VkDeviceQueueCreateInfo* pQueueInfo{ queueInfo }; pQueueInfo != pQueueInfoEnd; ++pQueueInfo) {
			pQueueInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			pQueueInfo->pNext = nullptr;
			pQueueInfo->flags = 0;
			pQueueInfo->queueFamilyIndex = *pFamily++;
			pQueueInfo->queueCount = 1u;
			pQueueInfo->pQueuePriorities = &queuePriority;
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
		createInfo.queueCreateInfoCount = queueInfoCount;
		createInfo.pQueueCreateInfos = queueInfo;
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.pEnabledFeatures = &enabledFeatures;

		if (vkCreateDevice(renderer->physicalDevice, &createInfo, nullptr, &renderer->device) != VK_SUCCESS) {
			pScratch->restore();
			goto failure_1;
		}

		pScratch->restore();
	}

	vkGetDeviceQueue(renderer->device, renderer->queueFamily.graphics, 0, &renderer->queue.graphics);
	vkGetDeviceQueue(renderer->device, renderer->queueFamily.transfer, 0, &renderer->queue.transfer);
	vkGetDeviceQueue(renderer->device, renderer->queueFamily.present, 0, &renderer->queue.present);

	{
		VmaAllocatorCreateInfo createInfo{};
		createInfo.flags = VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT;
		createInfo.physicalDevice = renderer->physicalDevice;
		createInfo.device = renderer->device;
		createInfo.preferredLargeHeapBlockSize = 0;
		createInfo.pAllocationCallbacks = nullptr;
		createInfo.pDeviceMemoryCallbacks = nullptr;
		createInfo.pHeapSizeLimit = nullptr;
		createInfo.pVulkanFunctions = nullptr;
		createInfo.instance = context->instance;
		createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		createInfo.pTypeExternalMemoryHandleTypes = nullptr;

		if (vmaCreateAllocator(&createInfo, &renderer->allocator) != VK_SUCCESS)
			goto failure_2;
	}

	{
		constexpr VkSurfaceFormatKHR preferredSurfaceFormats[] {
			{ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
			{ VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
			{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
			{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }
		};

		uint32_t surfaceFormatCount;

		if (vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->physicalDevice, renderer->surface, &surfaceFormatCount, nullptr) != VK_SUCCESS)
			goto failure_3;

		pScratch->frame();

		VkSurfaceFormatKHR* const pSurfaceFormat = mem_stackAllocateRange<VkSurfaceFormatKHR>(pScratch, (size_t)surfaceFormatCount);

		if (!pSurfaceFormat) {
			pScratch->restore();
			goto failure_3;
		}

		if (vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->physicalDevice, renderer->surface, &surfaceFormatCount, pSurfaceFormat) != VK_SUCCESS) {
			pScratch->restore();
			goto failure_3;
		}

		const VkSurfaceFormatKHR* pFormat = nullptr;

		VkSurfaceFormatKHR* const pSurfaceFormatEnd = pSurfaceFormat + surfaceFormatCount;

		const VkSurfaceFormatKHR* const pPreferredFormatEnd = preferredSurfaceFormats + 4u;
		for (const VkSurfaceFormatKHR* pPreferredFormat{ preferredSurfaceFormats }; pPreferredFormat != pPreferredFormatEnd; ++pPreferredFormat) {
			pFormat = std::find_if(pSurfaceFormat, pSurfaceFormatEnd, [pPreferredFormat](const VkSurfaceFormatKHR& _Format) { return _Format.format == pPreferredFormat->format && _Format.colorSpace == pPreferredFormat->colorSpace; });

			if (pFormat != pSurfaceFormatEnd)
				break;

			pFormat = nullptr;
		}

		if (!pFormat) {
			pScratch->restore();
			goto failure_3;
		}

		renderer->format = *pFormat;

		uint32_t surfacePresentModeCount;

		if (vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->physicalDevice, renderer->surface, &surfacePresentModeCount, nullptr) != VK_SUCCESS) {
			pScratch->restore();
			goto failure_3;
		}

		VkPresentModeKHR* const pSurfacePresentMode = mem_stackAllocateRange<VkPresentModeKHR>(pScratch, (size_t)surfacePresentModeCount);	

		if (!pSurfacePresentMode) {
			pScratch->restore();
			goto failure_3;
		}

		if (vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->physicalDevice, renderer->surface, &surfacePresentModeCount, pSurfacePresentMode) != VK_SUCCESS) {
			pScratch->restore();
			goto failure_3;
		}

		VkPresentModeKHR* const pSurfacePresentModeEnd = pSurfacePresentMode + surfacePresentModeCount;
		VkPresentModeKHR* pPresentMode = std::find(pSurfacePresentMode, pSurfacePresentModeEnd, VK_PRESENT_MODE_MAILBOX_KHR);

		renderer->presentMode = pPresentMode != pSurfacePresentModeEnd ? *pPresentMode : VK_PRESENT_MODE_FIFO_KHR;

		pScratch->restore();
	}

	{
		VkSurfaceCapabilitiesKHR capability;

		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderer->physicalDevice, renderer->surface, &capability) != VK_SUCCESS)
			goto failure_3;

		uint32_t imageCount = std::max<uint32_t>(pCreateInfo->imageCount, capability.minImageCount);

		if (capability.maxImageCount)
			imageCount = std::min<uint32_t>(imageCount, capability.maxImageCount);

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.surface = renderer->surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = renderer->format.format;
		createInfo.imageColorSpace = renderer->format.colorSpace;
		createInfo.imageExtent = capability.currentExtent;
		createInfo.imageArrayLayers = 1u;
		createInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
		createInfo.preTransform = capability.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = renderer->presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(renderer->device, &createInfo, nullptr, &renderer->swapchain) != VK_SUCCESS)
			goto failure_3;

		renderer->extent = capability.currentExtent;
		renderer->layers = 1u;
	}

	if (vkGetSwapchainImagesKHR(renderer->device, renderer->swapchain, &renderer->imageCount, nullptr))
		goto failure_4;

	renderer->image = mem_allocateRange<VkImage>(renderer->imageCount);

	if (!renderer->image)
		goto failure_4;

	if (vkGetSwapchainImagesKHR(renderer->device, renderer->swapchain, &renderer->imageCount, renderer->image))
		goto failure_5;

	{
		constexpr VkFormat preferredDepthFormat[] = {
			VK_FORMAT_D32_SFLOAT
		};

		const VkFormat* pDepthFormat = nullptr;

		const VkFormat* const pPreferredDepthFormatEnd = preferredDepthFormat + 1u;
		for (const VkFormat* pPreferredDepthFormat{ preferredDepthFormat }; pPreferredDepthFormat != pPreferredDepthFormatEnd; ++pPreferredDepthFormat) {
			VkFormatProperties formatProp;
			vkGetPhysicalDeviceFormatProperties(renderer->physicalDevice, *pPreferredDepthFormat, &formatProp);

			if (formatProp.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
				pDepthFormat = pPreferredDepthFormat;
				break;
			}
		}

		if (!pDepthFormat)
			goto failure_5;

		renderer->depthFormat = *pDepthFormat;
	}

	{
		VkImageCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.imageType = VK_IMAGE_TYPE_2D;
		createInfo.format = renderer->depthFormat;
		createInfo.extent = { renderer->extent.width, renderer->extent.height, 1u };
		createInfo.mipLevels = 1u;
		createInfo.arrayLayers = 1u;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0u;
		createInfo.pQueueFamilyIndices = nullptr;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VmaAllocationCreateInfo allocationCreateInfo{};
		allocationCreateInfo.flags = 0;
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocationCreateInfo.requiredFlags = 0;
		allocationCreateInfo.preferredFlags = 0;
		allocationCreateInfo.memoryTypeBits = 0;
		allocationCreateInfo.pool = VK_NULL_HANDLE;
		allocationCreateInfo.pUserData = nullptr;
		allocationCreateInfo.priority = 0.0f;

		if (vmaCreateImage(renderer->allocator, &createInfo, &allocationCreateInfo, &renderer->depthImage, &renderer->depthImageAllocation, nullptr) != VK_SUCCESS)
			goto failure_5;
	}
			
	renderer->imageView = mem_allocateRange<VkImageView>(renderer->imageCount);

	if (!renderer->imageView)
		goto failure_6;

	mem_nullifyRange<VkImageView>(renderer->imageView, renderer->imageCount);

	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = renderer->format.format;
		createInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		createInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		const VkImage* pImage = renderer->image;

		const VkImageView* const pImageViewEnd = renderer->imageView + renderer->imageCount;
		for (VkImageView* pImageView{ renderer->imageView }; pImageView != pImageViewEnd; ++pImageView) {
			createInfo.image = *pImage++;
			if (vkCreateImageView(renderer->device, &createInfo, nullptr, pImageView) != VK_SUCCESS)
				goto failure_7;
		}
	}

	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.image = renderer->depthImage;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = renderer->depthFormat;
		createInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
		createInfo.subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };

		if (vkCreateImageView(renderer->device, &createInfo, nullptr, &renderer->depthImageView) != VK_SUCCESS)
			goto failure_7;
	}

	renderer->imageFence = mem_allocateRange<VkFence>(renderer->imageCount);
		
	if (!renderer->imageFence)
		goto failure_8;

	mem_nullifyRange<VkFence>(renderer->imageFence, renderer->imageCount);

	if (createBasePassResources(renderer->device, renderer->format.format, renderer->depthFormat, pScratch, &renderer->pass))
		goto failure_9;

	renderer->framebuffer = mem_allocateRange<VkFramebuffer>(renderer->imageCount);

	if (!renderer->framebuffer)
		goto failure_10;

	mem_nullifyRange<VkFramebuffer>(renderer->framebuffer, renderer->imageCount);

	{
		VkImageView attachment[2]{};
		attachment[1] = renderer->depthImageView;
		
		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.renderPass = renderer->pass.renderPass;
		createInfo.attachmentCount = 2u;
		createInfo.pAttachments = attachment;		
		createInfo.width = renderer->extent.width;
		createInfo.height = renderer->extent.height;
		createInfo.layers = 1u;
		
		const VkImageView* pImageView = renderer->imageView;
		VkFramebuffer* pFramebuffer = renderer->framebuffer;

		const VkFramebuffer* const pFramebufferEnd = renderer->framebuffer + renderer->imageCount;
		while (pFramebuffer != pFramebufferEnd) {
			attachment[0] = *pImageView++;
			if (vkCreateFramebuffer(renderer->device, &createInfo, nullptr, pFramebuffer++) != VK_SUCCESS)
				goto failure_11;
		}
	}

	renderer->acquire = UINT32_MAX;
	
	renderer->imageAvailable = mem_allocateRange<VkSemaphore>(pCreateInfo->renderProcess);

	if (!renderer->imageAvailable)
		goto failure_11;

	mem_nullifyRange<VkSemaphore>(renderer->imageAvailable, pCreateInfo->renderProcess);

	{
		VkSemaphoreCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;

		const VkSemaphore* const pSemaphoreEnd = renderer->imageAvailable + pCreateInfo->renderProcess;
		for (VkSemaphore* pSemaphore{ renderer->imageAvailable }; pSemaphore != pSemaphoreEnd; ++pSemaphore)
			if (vkCreateSemaphore(renderer->device, &createInfo, nullptr, pSemaphore) != VK_SUCCESS)
				goto failure_12;
	}

	if (createSubmissionResource(renderer->device, renderer->queueFamily.graphics, pCreateInfo->renderProcess, &renderer->graphics))
		goto failure_12;

	{
		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.size = VkDeviceSize(pCreateInfo->stageCapacity);
		createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
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
		if (vmaCreateBuffer(renderer->allocator, &createInfo, &allocationCreateInfo, &renderer->stagingBuffer, &renderer->stagingAllocation, &allocationInfo) != VK_SUCCESS)
			goto failure_13;

		renderer->stage = { reinterpret_cast<uint8_t*>(allocationInfo.pMappedData), (size_t)allocationInfo.size };
	}

	renderer->region = mem_allocateRange<uint8_t*>(pCreateInfo->transferProcess);

	if (!renderer->region)
		goto failure_14;

	mem_nullifyRange<uint8_t*>(renderer->region, pCreateInfo->transferProcess);

	if (createSubmissionResource(renderer->device, renderer->queueFamily.transfer, pCreateInfo->transferProcess, &renderer->transfer))
		goto failure_15;

	*pRenderer = renderer;

	return 0;

failure_15:
	mem_freeRange<uint8_t*>(renderer->region);
	
failure_14:
	vmaDestroyBuffer(renderer->allocator, renderer->stagingBuffer, renderer->stagingAllocation);

failure_13:
	destroySubmissionResource(renderer->device, &renderer->graphics);

failure_12:
	{
		const VkSemaphore* const pSemaphoreEnd = renderer->imageAvailable + pCreateInfo->renderProcess;
		for (const VkSemaphore* pSemaphore{ renderer->imageAvailable }; pSemaphore != pSemaphoreEnd && *pSemaphore; ++pSemaphore)
			vkDestroySemaphore(renderer->device, *pSemaphore, nullptr);
	}

	mem_freeRange<VkSemaphore>(renderer->imageAvailable);
	
failure_11:
	{
		const VkFramebuffer* const pFramebufferEnd = renderer->framebuffer + renderer->imageCount;
		for (const VkFramebuffer* pFramebuffer{ renderer->framebuffer }; pFramebuffer != pFramebufferEnd && *pFramebuffer; ++pFramebuffer)
			vkDestroyFramebuffer(renderer->device, *pFramebuffer, nullptr);

	}
	
	mem_freeRange<VkFramebuffer>(renderer->framebuffer);

failure_10:
	destroyBasePassResources(renderer->device, &renderer->pass);

failure_9:
	mem_freeRange<VkFence>(renderer->imageFence);

failure_8:
	vkDestroyImageView(renderer->device, renderer->depthImageView, nullptr);

failure_7:
	{
		const VkImageView* const pImageViewEnd = renderer->imageView + renderer->imageCount;
		for (const VkImageView* pImageView{ renderer->imageView }; pImageView != pImageViewEnd && *pImageView; ++pImageView)
			vkDestroyImageView(renderer->device, *pImageView, nullptr);
	}

	mem_freeRange<VkImageView>(renderer->imageView);

failure_6:
	vmaDestroyImage(renderer->allocator, renderer->depthImage, renderer->depthImageAllocation);

failure_5:
	mem_freeRange<VkImage>(renderer->image);

failure_4:
	vkDestroySwapchainKHR(renderer->device, renderer->swapchain, nullptr);

failure_3:
	vmaDestroyAllocator(renderer->allocator);

failure_2:
	vkDestroyDevice(renderer->device, nullptr);

failure_1:
	vkDestroySurfaceKHR(context->instance, renderer->surface, nullptr);

failure_0:
	delete renderer;

	return -1;
}

void destroyRenderDevice(VulkanContext const context, __Renderer const renderer) noexcept {
	destroySubmissionResource(renderer->device, &renderer->transfer);

	mem_freeRange<uint8_t*>(renderer->region);	
	vmaDestroyBuffer(renderer->allocator, renderer->stagingBuffer, renderer->stagingAllocation);

	destroySubmissionResource(renderer->device, &renderer->graphics);

	const VkSemaphore* const pSemaphoreEnd = renderer->imageAvailable + renderer->graphics.capacity;
	for (const VkSemaphore* pSemaphore{ renderer->imageAvailable }; pSemaphore != pSemaphoreEnd && *pSemaphore; ++pSemaphore)
		vkDestroySemaphore(renderer->device, *pSemaphore, nullptr);

	const VkFramebuffer* const pFramebufferEnd = renderer->framebuffer + renderer->imageCount;
	for (const VkFramebuffer* pFramebuffer{ renderer->framebuffer }; pFramebuffer != pFramebufferEnd; ++pFramebuffer)
		vkDestroyFramebuffer(renderer->device, *pFramebuffer, nullptr);

	mem_freeRange<VkSemaphore>(renderer->imageAvailable);
	
	destroyBasePassResources(renderer->device, &renderer->pass);
	mem_freeRange<VkFence>(renderer->imageFence);

	vkDestroyImageView(renderer->device, renderer->depthImageView, nullptr);

	const VkImageView* const pImageViewEnd = renderer->imageView + renderer->imageCount;
	for (const VkImageView* pImageView{ renderer->imageView }; pImageView != pImageViewEnd; ++pImageView)
		vkDestroyImageView(renderer->device, *pImageView, nullptr);

	mem_freeRange<VkImageView>(renderer->imageView);

	vmaDestroyImage(renderer->allocator, renderer->depthImage, renderer->depthImageAllocation);

	mem_freeRange<VkImage>(renderer->image);

	vkDestroySwapchainKHR(renderer->device, renderer->swapchain, nullptr);

	vmaDestroyAllocator(renderer->allocator);
	vkDestroyDevice(renderer->device, nullptr);

	vkDestroySurfaceKHR(context->instance, renderer->surface, nullptr);

	delete renderer;
}

int waitRenderDevice(const __Renderer renderer) noexcept {
	if (vkDeviceWaitIdle(renderer->device) == VK_SUCCESS)
		return 0;

	return -1;
}

int configureRenderDevice(const __Renderer renderer) noexcept {
	VkExtent2D extent;
	uint32_t count;

	VkSwapchainKHR _swapchain ;
	VkImage* _image;
	VkImage _depthImage;
	VmaAllocation _depthImageAllocation;
	VkImageView* _imageView = nullptr;
	VkImageView _depthImageView;
	VkFence* _fence;
	VkFramebuffer* _framebuffer;

	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;

		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderer->physicalDevice, renderer->surface, &surfaceCapabilities))
			return -1;

		uint32_t minImageCount = static_cast<uint32_t>(renderer->imageCount);

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
			createInfo.surface = renderer->surface;
			createInfo.minImageCount = minImageCount;
			createInfo.imageFormat = renderer->format.format;
			createInfo.imageColorSpace = renderer->format.colorSpace;
			createInfo.imageExtent = surfaceCapabilities.currentExtent;
			createInfo.imageArrayLayers = 1u;
			createInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
			createInfo.preTransform = surfaceCapabilities.currentTransform;
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.presentMode = renderer->presentMode;
			createInfo.clipped = VK_TRUE;
			createInfo.oldSwapchain = renderer->swapchain;

			if (vkCreateSwapchainKHR(renderer->device, &createInfo, nullptr, &_swapchain) != VK_SUCCESS)
				return -1;
		}

		extent = surfaceCapabilities.currentExtent;
	}

	if (vkGetSwapchainImagesKHR(renderer->device, _swapchain, &count, nullptr))
		goto failure_0;

	_image = mem_allocateRange<VkImage>(count);

	if (!_image)
		goto failure_0;

	if (vkGetSwapchainImagesKHR(renderer->device, _swapchain, &count, _image) != VK_SUCCESS)
		goto failure_1;

	{
		VkImageCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.imageType = VK_IMAGE_TYPE_2D;
		createInfo.format = renderer->depthFormat;
		createInfo.extent = { extent.width, extent.height, 1u };
		createInfo.mipLevels = 1u;
		createInfo.arrayLayers = 1u;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0u;
		createInfo.pQueueFamilyIndices = nullptr;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VmaAllocationCreateInfo allocationCreateInfo{};
		allocationCreateInfo.flags = 0;
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocationCreateInfo.requiredFlags = 0;
		allocationCreateInfo.preferredFlags = 0;
		allocationCreateInfo.memoryTypeBits = 0;
		allocationCreateInfo.pool = VK_NULL_HANDLE;
		allocationCreateInfo.pUserData = nullptr;
		allocationCreateInfo.priority = 0.0f;

		if (vmaCreateImage(renderer->allocator, &createInfo, &allocationCreateInfo, &_depthImage, &_depthImageAllocation, nullptr) != VK_SUCCESS)
			goto failure_3;
	}
	
	_imageView = mem_allocateRange<VkImageView>(count);

	if (!_imageView)
		goto failure_2;

	mem_nullifyRange(_imageView, count);

	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = renderer->format.format;
		createInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		createInfo.subresourceRange= { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		const VkImage* pImage = _image;

		const VkImageView* const pImageViewEnd = _imageView + count;
		for (VkImageView* pImageView{ _imageView }; pImageView != pImageViewEnd; ++pImageView) {
			createInfo.image = *pImage++;
			if (vkCreateImageView(renderer->device, &createInfo, nullptr, pImageView) != VK_SUCCESS)
				goto failure_3;
		}
	}

	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.image = _depthImage;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = renderer->depthFormat;
		createInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
		createInfo.subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };

		if (vkCreateImageView(renderer->device, &createInfo, nullptr, &_depthImageView) != VK_SUCCESS)
			goto failure_4;
	}

	_fence = mem_allocateRange<VkFence>(count);

	if (!_fence)
		goto failure_4;

	mem_nullifyRange(_fence, count);

	_framebuffer = mem_allocateRange<VkFramebuffer>(count);

	if (!_framebuffer)
		goto failure_5;

	mem_nullifyRange<VkFramebuffer>(_framebuffer, count);

	{
		VkImageView attachment[2]{};
		attachment[1] = _depthImageView;

		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.renderPass = renderer->pass.renderPass;
		createInfo.attachmentCount = 2u;
		createInfo.pAttachments = attachment;
		createInfo.width = extent.width;
		createInfo.height = extent.height;
		createInfo.layers = 1u;

		VkFramebuffer* pFramebuffer = _framebuffer;
		const VkImageView* pImageView = _imageView;

		const VkFramebuffer* const pFramebufferEnd = _framebuffer + count;
		while (pFramebuffer != pFramebufferEnd) {
			attachment[0] = *pImageView++;
			if (vkCreateFramebuffer(renderer->device, &createInfo, nullptr, pFramebuffer++) != VK_SUCCESS)
				goto failure_6;
		}
	}

	{
		const VkFence* const pFenceEnd = renderer->imageFence + renderer->imageCount;
		for (const VkFence* pFence{ renderer->imageFence }; pFence != pFenceEnd; ++pFence) {
			if (!*pFence)
				continue;

			if (vkWaitForFences(renderer->device, 1u, pFence, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
				goto failure_6;
		}
	}

	{
		const VkFramebuffer* const pFramebufferEnd = renderer->framebuffer + renderer->imageCount;
		for (const VkFramebuffer* pFramebuffer{ renderer->framebuffer }; pFramebuffer != pFramebufferEnd; ++pFramebuffer)
			vkDestroyFramebuffer(renderer->device, *pFramebuffer, nullptr);

		mem_freeRange<VkFramebuffer>(renderer->framebuffer);

		mem_freeRange<VkFence>(renderer->imageFence);
		
		vkDestroyImageView(renderer->device, renderer->depthImageView, nullptr);

		const VkImageView* const pImageViewEnd = renderer->imageView + renderer->imageCount;
		for (const VkImageView* pImageView{ renderer->imageView }; pImageView != pImageViewEnd; ++pImageView)
			vkDestroyImageView(renderer->device, *pImageView, nullptr);

		mem_freeRange<VkImageView>(renderer->imageView);

		vmaDestroyImage(renderer->allocator, renderer->depthImage, renderer->depthImageAllocation);				

		mem_freeRange<VkImage>(renderer->image);

		vkDestroySwapchainKHR(renderer->device, renderer->swapchain, nullptr);
	}

	renderer->framebuffer = _framebuffer;
	renderer->imageFence = _fence;
	renderer->depthImageView = _depthImageView;
	renderer->imageView = _imageView;
	renderer->depthImageAllocation = _depthImageAllocation;
	renderer->depthImage = _depthImage;
	renderer->image = _image;
	renderer->swapchain = _swapchain;
	renderer->imageCount = count;
	renderer->extent = extent;

	return 0;

failure_6:
	{
		const VkFramebuffer* const pFramebufferEnd = _framebuffer + count;
		for (const VkFramebuffer* pFramebuffer{ _framebuffer }; pFramebuffer != pFramebufferEnd; ++pFramebuffer)
			vkDestroyFramebuffer(renderer->device, *pFramebuffer, nullptr);
	}

	mem_freeRange<VkFramebuffer>(_framebuffer);

failure_5:
	mem_freeRange<VkFence>(_fence);

failure_4:
	vkDestroyImageView(renderer->device, _depthImageView, nullptr);

failure_3:
	{
		const VkImageView* const pImageViewEnd = _imageView + count;
		for (const VkImageView* pImageView{ _imageView }; pImageView != pImageViewEnd && *pImageView; ++pImageView)
			vkDestroyImageView(renderer->device, *pImageView, nullptr);
	}

	mem_freeRange<VkImageView>(_imageView);

failure_2:
	vmaDestroyImage(renderer->allocator, _depthImage, _depthImageAllocation);

failure_1:
	mem_freeRange<VkImage>(_image);

failure_0:
	vkDestroySwapchainKHR(renderer->device, _swapchain, nullptr);

	return -1;
}

static int prepareRoundedSubmission(const VkDevice device, const SubmissionResource* const pSubmission, NULLABLE uint32_t* const pIndex) noexcept {
	uint32_t index = pSubmission->index;

	if (vkWaitForFences(device, 1u, &pSubmission->fence[index], VK_TRUE, UINT64_MAX) != VK_SUCCESS)
		return -1;

	if (pIndex)
		*pIndex = index;

	return 0;
}

static inline void proceedRoundedSubmission(SubmissionResource* const pSubmission) noexcept {
	const uint32_t index = pSubmission->index + 1u;

	if (index == pSubmission->capacity)
		pSubmission->index = 0u;
	else 
		pSubmission->index = index;
}

static int prepareStackedSubmission(const VkDevice device, SubmissionResource* const pSubmission, NULLABLE uint32_t* const pIndex) noexcept {
	uint32_t index = pSubmission->index;
	const uint32_t capacity = pSubmission->capacity;

	if (!index) {
		if (pIndex)
			*pIndex = 0u;
		
		return 0;
	}

	while (index) {
		--index;

		switch (vkGetFenceStatus(device, pSubmission->fence[index])) {
		case VK_SUCCESS:
			break;
		case VK_NOT_READY:
			++index;
			goto next;
		default:
			return -1;
		}
	}

next:
	if (index >= capacity) {
		const VkResult result = vkWaitForFences(device, capacity, pSubmission->fence, VK_TRUE, UINT64_MAX);

		if (result != VK_SUCCESS)
			return -1;

		pSubmission->index = 0u;

		if (pIndex)
			*pIndex = 0u;
		return 0;
	}

	pSubmission->index = index;

	if (pIndex)
		*pIndex = index;

	return 0;
}

static inline void proceedStackedSubmission(SubmissionResource* const pSubmission) noexcept {
	pSubmission->index++;
}

static int waitRoundedSubmission(const VkDevice device, const SubmissionResource* const pSubmission) noexcept {
	const uint32_t capacity = pSubmission->capacity;

	if (vkWaitForFences(device, capacity, pSubmission->fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
		return -1;

	return 0;
}

static int waitStackedSubmission(const VkDevice device, const SubmissionResource* const pSubmission) noexcept {
	const uint32_t count = pSubmission->index;
	
	if (!count)
		return 0;

	if (vkWaitForFences(device, count, pSubmission->fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
		return -1;

	return 0;
}

static int updateStagingCapacity(const __Renderer renderer, const size_t size) noexcept {
	VkBuffer buffer;
	VmaAllocation allocation;

	VkBufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.size = size;
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
	if (vmaCreateBuffer(renderer->allocator, &createInfo, &allocationCreateInfo, &buffer, &allocation, &allocationInfo) != VK_SUCCESS)
		return -1;

	if (waitRoundedSubmission(renderer->device, &renderer->transfer)) {
		vmaDestroyBuffer(renderer->allocator, buffer, allocation);
		return -1;
	}

	renderer->stage = { reinterpret_cast<uint8_t*>(allocationInfo.pMappedData), (size_t)allocationInfo.size };
	renderer->stagingBuffer = buffer;
	renderer->stagingAllocation = allocation;
	
	return 0;
}

struct Model_T {
	uint32_t firstVertex;
	uint32_t firstIndex;
	uint32_t indexcount;
};

struct Collection_T {
	Model_T* model;
	VkBuffer vertex;
	VmaAllocation vertexAllocation;
	VkBuffer index;
	VmaAllocation indexAllocation;
};

int createCollection(const __Renderer renderer, const CollectionCreateInfo* const pCreateInfo, Collection* const pCollection) noexcept {
	const Collection collection = new(std::nothrow) Collection_T;

	if (!collection)
		return -1;
	
	const size_t modelCount = (size_t)pCreateInfo->modelCount;
	
	if (!modelCount)
		goto failure_0;
		
	collection->model = mem_allocateSizeRange<Model_T>(modelCount);
		
	if (!collection->model)
		goto failure_0;
		
	{
		uint32_t totalVertexCount = 0;
		uint32_t totalIndexCount = 0;
			
		uint32_t maxVertexCount = 0;
		uint32_t maxIndexCount = 0;

		Model_T* pModel = collection->model;

		const ModelInfo* const pModelInfoEnd = pCreateInfo->pModelInfos + modelCount;
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
		
		if (renderer->stage.count < maxAllocationSize && updateStagingCapacity(renderer, maxAllocationSize))
			goto failure_1;

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

			if (vmaCreateBuffer(renderer->allocator, &createInfo, &allocationCreateInfo, &collection->vertex, &collection->vertexAllocation, nullptr) != VK_SUCCESS)
				goto failure_1;
		}

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

			if (vmaCreateBuffer(renderer->allocator, &createInfo, &allocationCreateInfo, &collection->index, &collection->indexAllocation, nullptr) != VK_SUCCESS)
				goto failure_2;
		}
	}

	{
		VkDeviceSize vertexOffset = 0;
		VkDeviceSize indexOffset = 0;

		const ModelInfo* const pModelInfoEnd = pCreateInfo->pModelInfos + pCreateInfo->modelCount;
		for (const ModelInfo* pModelInfo{ pCreateInfo->pModelInfos }; pModelInfo != pModelInfoEnd;) {
			uint8_t* head;
			uint8_t** pRegion;

			{
				uint32_t index;
				if (prepareStackedSubmission(renderer->device, &renderer->transfer, &index))
					goto failure_3;

				head = index ? renderer->region[index - 1u] : renderer->stage.data;

				pRegion = renderer->region + index;
			}

			const VkCommandBuffer commandBuffer = renderer->transfer.thisCommandBuffer();

			if (vkResetCommandBuffer(commandBuffer, 0) != VK_SUCCESS)
				goto failure_3;

			{
				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.pNext = nullptr;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
				beginInfo.pInheritanceInfo = nullptr;

				if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
					goto failure_3;
			}

			uint8_t* const base = head;

			do {
				const size_t vertexSize = sizeof(Vertex) * pModelInfo->vertexCount;
				const size_t indexSize = sizeof(Index) * pModelInfo->indexCount;
				
				const size_t allocationSize = vertexSize + indexSize;
				
				if (static_cast<size_t>(renderer->stage.end() - head) < allocationSize)
					break;
				
				memcpy(head, pModelInfo->pVertex, vertexSize);

				{
					VkBufferCopy copy{};
					copy.srcOffset = static_cast<VkDeviceSize>(head - renderer->stage.data);
					copy.dstOffset = vertexOffset;
					copy.size = vertexSize;

					vkCmdCopyBuffer(commandBuffer, renderer->stagingBuffer, collection->vertex, 1u, &copy);
				}

				head += vertexSize;
				vertexOffset += vertexSize;

				memcpy(head, pModelInfo->pIndex, indexSize);

				{
					VkBufferCopy copy{};
					copy.srcOffset = static_cast<VkDeviceSize>(head - renderer->stage.data);
					copy.dstOffset = indexOffset;
					copy.size = indexSize;

					vkCmdCopyBuffer(commandBuffer, renderer->stagingBuffer, collection->index, 1u, &copy);
				}

				head += indexSize;
				indexOffset += indexSize;

				++pModelInfo;
			} while (pModelInfo != pModelInfoEnd);

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
				goto failure_3;

			if (head == base) {
				if (waitStackedSubmission(renderer->device, &renderer->transfer))
					goto failure_3;
				
				continue;
			}

			if (vmaFlushAllocation(renderer->allocator, renderer->stagingAllocation, static_cast<VkDeviceSize>(base - renderer->stage.data), static_cast<VkDeviceSize>(head - base)) != VK_SUCCESS)
				goto failure_3;

			const VkFence fence = renderer->transfer.thisFence();

			if (vkResetFences(renderer->device, 1u, &fence) != VK_SUCCESS)
				goto failure_3;

			{
				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.pNext = nullptr;
				submitInfo.waitSemaphoreCount = 0u;
				submitInfo.pWaitSemaphores = nullptr;
				submitInfo.pWaitDstStageMask = nullptr;
				submitInfo.commandBufferCount = 1u;
				submitInfo.pCommandBuffers = &commandBuffer;
				submitInfo.signalSemaphoreCount = 0u;;
				submitInfo.pSignalSemaphores = nullptr;

				if (vkQueueSubmit(renderer->queue.transfer, 1u, &submitInfo, fence) != VK_SUCCESS)
					goto failure_3;
			}

			*pRegion = head;

			proceedStackedSubmission(&renderer->transfer);

			if (waitStackedSubmission(renderer->device, &renderer->transfer))
				goto failure_3;
		}
	}

	*pCollection = collection;

	return 0;

failure_3:
	vmaDestroyBuffer(renderer->allocator, collection->index, collection->indexAllocation);

failure_2:
	vmaDestroyBuffer(renderer->allocator, collection->vertex, collection->vertexAllocation);

failure_1:
	mem_freeSizeRange<Model_T>(collection->model);

failure_0:
	delete collection;

	return -1;
}

void destroyCollection(const __Renderer renderer, Collection const _Collection) noexcept {
	vmaDestroyBuffer(renderer->allocator, _Collection->index, _Collection->indexAllocation);
	vmaDestroyBuffer(renderer->allocator, _Collection->vertex, _Collection->vertexAllocation);

	mem_freeSizeRange<Model_T>(_Collection->model);

	delete _Collection;
}

using ResourceStateFlags = uint32_t;
enum ResourceStateBit : ResourceStateFlags {
	RESOURCE_STATE_PENDING_TRANSFER_ACQUIRE = 1u << 0,
	RESOURCE_STATE_PENDING_TRANSFER_WAIT = 1u << 1
};

struct Texture_T {
	ResourceStateFlags state;

	VkImage image;
	VmaAllocation allocation;
	VkImageView imageView;

	VkDescriptorPool descriptorPool;
	VkDescriptorSet* descriptorSet;
};

int createTexture(const __Renderer renderer, const TextureCreateInfo* pCreateInfo, mem_stack* pScratch, Texture* pTexture) noexcept {
	const Texture texture = new(std::nothrow) Texture_T;

	if (!texture)
		return -1;

	size_t fileSize;
	uint8_t* imageBin;
	io::ImageInfo info;
	size_t imageSize;

	{
		fileSize = 0u;
		if (io::getBinarySize(pCreateInfo->path, &fileSize))
			goto failure_0;

		pScratch->frame();

		imageBin = mem_stackAllocateRange<uint8_t>(pScratch, fileSize);

		if (!imageBin) {
			pScratch->restore();
			goto failure_0;
		}

		if (io::loadBinary(pCreateInfo->path, fileSize, imageBin)) {
			pScratch->restore();
			goto failure_0;
		}

		if (io::fetchPngInfo(imageBin, fileSize, &info)) {
			pScratch->restore();
			goto failure_0;
		}

		imageSize = io::getImageSize(&info);
	}

	{
		VkImageCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.imageType = VK_IMAGE_TYPE_2D;
		createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		createInfo.extent = { info.width, info.height, 1u };
		createInfo.mipLevels = 1u;
		createInfo.arrayLayers = 1u;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0u;
		createInfo.pQueueFamilyIndices = nullptr;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VmaAllocationCreateInfo allocationCreateInfo{};
		allocationCreateInfo.flags = 0;
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocationCreateInfo.requiredFlags = 0;
		allocationCreateInfo.preferredFlags = 0;
		allocationCreateInfo.memoryTypeBits = 0;
		allocationCreateInfo.pool = VK_NULL_HANDLE;
		allocationCreateInfo.pUserData = nullptr;
		allocationCreateInfo.priority = 0.0f;

		if (vmaCreateImage(renderer->allocator, &createInfo, &allocationCreateInfo, &texture->image, &texture->allocation, nullptr) != VK_SUCCESS) {
			pScratch->restore();
			goto failure_0;
		}
	}

	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.image = texture->image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		createInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		createInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		if (vkCreateImageView(renderer->device, &createInfo, nullptr, &texture->imageView) != VK_SUCCESS) {
			pScratch->restore();
			goto failure_1;
		}
	}

	{
		VkDescriptorPoolSize size[1]{};
		size[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		size[0].descriptorCount = renderer->graphics.capacity;

		VkDescriptorPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.maxSets = renderer->graphics.capacity;
		createInfo.poolSizeCount = 1u;
		createInfo.pPoolSizes = size;

		if (vkCreateDescriptorPool(renderer->device, &createInfo, nullptr, &texture->descriptorPool) != VK_SUCCESS) {
			pScratch->restore();
			goto failure_2;
		}
	}

	texture->descriptorSet = mem_allocateRange<VkDescriptorSet>(renderer->graphics.capacity);

	if (!texture->descriptorSet) {
		pScratch->restore();
		goto failure_3;
	}

	{
		VkDescriptorSetLayout* setLayout = mem_stackAllocateRange<VkDescriptorSetLayout>(pScratch, renderer->graphics.capacity);

		if (!setLayout) {
			pScratch->restore();
			goto failure_4;
		}

		mem_assignRange(setLayout, renderer->graphics.capacity, renderer->pass.textureSetLayout);

		{
			VkDescriptorSetAllocateInfo allocateInfo{};
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.descriptorPool = texture->descriptorPool;
			allocateInfo.descriptorSetCount = renderer->graphics.capacity;
			allocateInfo.pSetLayouts = setLayout;		

			if (vkAllocateDescriptorSets(renderer->device, &allocateInfo, texture->descriptorSet) != VK_SUCCESS) {
				pScratch->restore();
				goto failure_4;
			}
		}
		
		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.sampler = VK_NULL_HANDLE;
			imageInfo.imageView = texture->imageView;
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.pNext = nullptr;
			write.dstBinding = 0u;
			write.dstArrayElement = 0u;
			write.descriptorCount = 1u;
			write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			write.pImageInfo = &imageInfo;
			write.pBufferInfo = nullptr;
			write.pTexelBufferView = nullptr;

			const VkDescriptorSet* const pDescriptorSetEnd = texture->descriptorSet + renderer->graphics.capacity;
			for (const VkDescriptorSet* pDescriptorSet{ texture->descriptorSet }; pDescriptorSet != pDescriptorSetEnd;) {
				write.dstSet = *pDescriptorSet++;

				vkUpdateDescriptorSets(renderer->device, 1u, &write, 0u, nullptr);
			}
		}
	}

	if (renderer->stage.count < imageSize && updateStagingCapacity(renderer, imageSize)) {
		pScratch->restore();
		goto failure_4;
	}

	{
		uint8_t* head;
		uint8_t** pRegion;

		{
		prepare:
			uint32_t index;
			if (prepareStackedSubmission(renderer->device, &renderer->transfer, &index)) {
				pScratch->restore();
				goto failure_4;
			}

			head = index ? renderer->region[index - 1u] : renderer->stage.data;

			if (static_cast<size_t>(renderer->stage.end() - head) < imageSize) {
				if (waitStackedSubmission(renderer->device, &renderer->transfer)) {
					pScratch->restore();
					goto failure_4;
				}

				goto prepare;
			}

			pRegion = renderer->region + index;
		}

		const VkCommandBuffer commandBuffer = renderer->transfer.thisCommandBuffer();

		if (vkResetCommandBuffer(commandBuffer, 0) != VK_SUCCESS) {
			pScratch->restore();
			goto failure_4;
		}

		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = nullptr;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			beginInfo.pInheritanceInfo = nullptr;

			if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
				pScratch->restore();
				goto failure_4;
			}
		}

		{
			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.pNext = nullptr;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = texture->image;
			barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1u, &barrier);
		}

		uint8_t* const base = head;

		{
			VkBufferImageCopy copy{};
			copy.bufferOffset = static_cast<VkDeviceSize>(head - renderer->stage.data);
			copy.bufferRowLength = 0u;
			copy.bufferImageHeight = 0u;
			copy.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
			copy.imageOffset = { 0, 0, 0 };
			copy.imageExtent = { info.width, info.height, 1u };

			io::ImageDecodeInfo decodeInfo{};
			decodeInfo.bin = imageBin;
			decodeInfo.binSize = fileSize;
			decodeInfo.imageInfo = &info;
			decodeInfo.pDst = head;

			if (io::decodePng(pScratch, &decodeInfo)) {
				vkEndCommandBuffer(commandBuffer);
				pScratch->restore();
				goto failure_4;
			}

			vkCmdCopyBufferToImage(commandBuffer, renderer->stagingBuffer, texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1u, &copy);
		}

		head += imageSize;

		pScratch->restore();

		{
			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.pNext = nullptr;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = 0;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcQueueFamilyIndex = renderer->queueFamily.transfer;
			barrier.dstQueueFamilyIndex = renderer->queueFamily.graphics;
			barrier.image = texture->image;
			barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1u, &barrier);
		}

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			goto failure_4;
		}

		if (vmaFlushAllocation(renderer->allocator, renderer->stagingAllocation, static_cast<VkDeviceSize>(base - renderer->stage.data), static_cast<VkDeviceSize>(head - base)) != VK_SUCCESS) {
			goto failure_4;
		}

		const VkFence fence = renderer->transfer.thisFence();

		if (vkResetFences(renderer->device, 1u, &fence) != VK_SUCCESS) {
			goto failure_4;
		}

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

			if (vkQueueSubmit(renderer->queue.transfer, 1u, &submitInfo, fence) != VK_SUCCESS) {
				goto failure_4;
			}
		}

		*pRegion = head;
		
		proceedStackedSubmission(&renderer->transfer);
	}
		
	if (waitStackedSubmission(renderer->device, &renderer->transfer))
		goto failure_4;
	
	texture->state = RESOURCE_STATE_PENDING_TRANSFER_ACQUIRE;
	
	*pTexture = texture;

	return 0;

failure_4:
	mem_freeRange<VkDescriptorSet>(texture->descriptorSet);

failure_3:
	vkDestroyDescriptorPool(renderer->device, texture->descriptorPool, nullptr);

failure_2:
	vkDestroyImageView(renderer->device, texture->imageView, nullptr);

failure_1:
	vmaDestroyImage(renderer->allocator, texture->image, texture->allocation);

failure_0:
	delete texture;
	
	return -1;
}

void destroyTexture(const __Renderer renderer, const Texture texture) noexcept {
	mem_freeRange<VkDescriptorSet>(texture->descriptorSet);
	vkDestroyDescriptorPool(renderer->device, texture->descriptorPool, nullptr);
	vkDestroyImageView(renderer->device, texture->imageView, nullptr);
	vmaDestroyImage(renderer->allocator, texture->image, texture->allocation);

	delete texture;
}

struct InstanceDataGPU {
	InstanceData external;
};

struct Scene_T {
	Model_T* model;
	VkBuffer* instance;
	VmaAllocation* instanceAllocation;
	InstanceDataGPU** instanceData;
	uint32_t instanceCount;
	uint32_t instanceCapacity;

	VkDescriptorPool descriptorPool;
	VkDescriptorSet* descriptorSet;

	VkBuffer* indirect;
	VmaAllocation* indirectAllocation;
	VkDrawIndexedIndirectCommand** indirectData;
	uint32_t indirectCommandCount;
	uint32_t indirectCommandCapacity;
};

int createScene(const __Renderer renderer, const SceneCreateInfo* const pCreateInfo, mem_stack* const pScratch, Scene* const pScene) noexcept {
	const Scene scene = new(std::nothrow) Scene_T;

	if (!scene)
		return -1;

	scene->model = pCreateInfo->collection->model;

	scene->instance = mem_allocateRange<VkBuffer>(renderer->graphics.capacity);

	if (!scene->instance)
		goto failure_0;

	mem_nullifyRange<VkBuffer>(scene->instance, renderer->graphics.capacity);
	
	scene->instanceAllocation = mem_allocateRange<VmaAllocation>(renderer->graphics.capacity);

	if (!scene->instanceAllocation)
		goto failure_2;

	scene->instanceData = mem_allocateRange<InstanceDataGPU*>(renderer->graphics.capacity);

	if (!scene->instanceData)
		goto failure_3;

	{
		const size_t instanceBufferSize = sizeof(InstanceDataGPU) * pCreateInfo->instanceCount;
		
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

		VkBuffer* pBuffer = scene->instance;
		VmaAllocation* pAllocation = scene->instanceAllocation;
		InstanceDataGPU** pData = scene->instanceData;

		const VkBuffer* const pBufferEnd = scene->instance + renderer->graphics.capacity;
		while (pBuffer != pBufferEnd) {
			if (vmaCreateBuffer(renderer->allocator, &createInfo, &allocationCreateInfo, pBuffer++, pAllocation++, &allocationInfo) != VK_SUCCESS)
				goto failure_4;

			*pData++ = reinterpret_cast<InstanceDataGPU*>(allocationInfo.pMappedData);
		}
	}

	{
		VkDescriptorPoolSize poolSize[1]{};
		poolSize[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSize[0].descriptorCount = renderer->graphics.capacity;

		VkDescriptorPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.maxSets = renderer->graphics.capacity;
		createInfo.poolSizeCount = 1u;
		createInfo.pPoolSizes = poolSize;

		if (vkCreateDescriptorPool(renderer->device, &createInfo, nullptr, &scene->descriptorPool) != VK_SUCCESS)
			goto failure_4;
	}

	scene->descriptorSet = mem_allocateRange<VkDescriptorSet>(renderer->graphics.capacity);

	if (!scene->descriptorSet)
		goto failure_5;

	{
		pScratch->frame();

		VkDescriptorSetLayout* setLayout = mem_stackAllocateRange<VkDescriptorSetLayout>(pScratch, renderer->graphics.capacity);

		if (!setLayout) {
			pScratch->restore();
			goto failure_6;
		}

		mem_assignRange<VkDescriptorSetLayout>(setLayout, renderer->graphics.capacity, renderer->pass.objectSetLayout);

		{
			VkDescriptorSetAllocateInfo allocateInfo{};
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.descriptorPool = scene->descriptorPool;
			allocateInfo.descriptorSetCount = renderer->graphics.capacity;
			allocateInfo.pSetLayouts = setLayout;

			if (vkAllocateDescriptorSets(renderer->device, &allocateInfo, scene->descriptorSet) != VK_SUCCESS) {
				pScratch->restore();
				goto failure_6;
			}
		}

		pScratch->restore();

		{
			const size_t instanceBufferSize = sizeof(InstanceDataGPU) * pCreateInfo->instanceCount;

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

			const VkBuffer* pBuffer = scene->instance;

			const VkDescriptorSet* const pDescriptorSetEnd = scene->descriptorSet + renderer->graphics.capacity;
			for (const VkDescriptorSet* pDescriptorSet{ scene->descriptorSet }; pDescriptorSet != pDescriptorSetEnd;) {
				bufferInfo.buffer = *pBuffer++;
				write.dstSet = *pDescriptorSet++;

				vkUpdateDescriptorSets(renderer->device, 1u, &write, 0u, nullptr);
			}
		}

		scene->instanceCapacity = pCreateInfo->instanceCount;
	}

	scene->instanceCount = 0u;

	scene->indirect = mem_allocateRange<VkBuffer>(renderer->graphics.capacity);

	if (!scene->indirect)
		goto failure_6;

	mem_nullifyRange<VkBuffer>(scene->indirect, renderer->graphics.capacity);

	scene->indirectAllocation = mem_allocateRange<VmaAllocation>(renderer->graphics.capacity);

	if (!scene->indirectAllocation)
		goto failure_7;
		
	scene->indirectData = mem_allocateRange<VkDrawIndexedIndirectCommand*>(renderer->graphics.capacity);

	if (!scene->indirectData)
		goto failure_8;

	{
		const uint32_t maxDrawCount = std::min<uint32_t>(pCreateInfo->instanceCount, std::max<uint32_t>(pCreateInfo->drawCount, static_cast<uint32_t>(mem_getSizeAllocationSize(pCreateInfo->collection->model) + 1u)));
		
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

		VkBuffer* pBuffer = scene->indirect;
		VmaAllocation* pAllocation = scene->indirectAllocation;
		VkDrawIndexedIndirectCommand** pData = scene->indirectData;

		const VkBuffer* const pBufferEnd = scene->indirect + renderer->graphics.capacity;
		while (pBuffer != pBufferEnd) {
			if (vmaCreateBuffer(renderer->allocator, &createInfo, &allocationCreateInfo, pBuffer++, pAllocation++, &allocationInfo) != VK_SUCCESS)
				goto failure_9;

			*pData++ = reinterpret_cast<VkDrawIndexedIndirectCommand*>(allocationInfo.pMappedData);
		}

		scene->indirectCommandCapacity = maxDrawCount;
	}

	scene->indirectCommandCount = 0u;

	*pScene = scene;

	return 0;

failure_9:
	{
		VkBuffer* pBuffer = scene->indirect;
		VmaAllocation* pAllocation = scene->indirectAllocation;

		const VkBuffer* const pBufferEnd = scene->indirect + renderer->graphics.capacity;
		while (pBuffer != pBufferEnd)
			vmaDestroyBuffer(renderer->allocator, *pBuffer, *pAllocation);
	}

	mem_freeRange<VkDrawIndexedIndirectCommand*>(scene->indirectData);

failure_8:
	mem_freeRange<VmaAllocation>(scene->indirectAllocation);

failure_7:
	mem_freeRange<VkBuffer>(scene->indirect);

failure_6:
	mem_freeRange<VkDescriptorSet>(scene->descriptorSet);

failure_5:
	vkDestroyDescriptorPool(renderer->device, scene->descriptorPool, nullptr);

failure_4:
	{
		VkBuffer* pBuffer = scene->instance;
		VmaAllocation* pAllocation = scene->instanceAllocation;

		const VkBuffer* const pBufferEnd = scene->instance + renderer->graphics.capacity;
		while (pBuffer != pBufferEnd && *pBuffer)
			vmaDestroyBuffer(renderer->allocator, *pBuffer, *pAllocation);
	}

	mem_freeRange<InstanceDataGPU*>(scene->instanceData);

failure_3:
	mem_freeRange<VmaAllocation>(scene->instanceAllocation);

failure_2:
	mem_freeRange<VkBuffer>(scene->instance);

failure_0:
	delete scene;

	return -1;
}

void destroyScene(const __Renderer renderer, const Scene scene) noexcept {
	mem_freeRange<VkDrawIndexedIndirectCommand*>(scene->indirectData);

	{
		const VkBuffer* pBuffer = scene->indirect;
		const VmaAllocation* pAllocation = scene->indirectAllocation;

		const VkBuffer* const pBufferEnd = scene->indirect + renderer->graphics.capacity;
		while (pBuffer != pBufferEnd)
			vmaDestroyBuffer(renderer->allocator, *pBuffer++, *pAllocation++);
	}

	mem_freeRange<VmaAllocation>(scene->indirectAllocation);
	mem_freeRange<VkBuffer>(scene->indirect);

	mem_freeRange<VkDescriptorSet>(scene->descriptorSet);
	vkDestroyDescriptorPool(renderer->device, scene->descriptorPool, nullptr);

	mem_freeRange<InstanceDataGPU*>(scene->instanceData);
	
	{
		const VkBuffer* pBuffer = scene->instance;
		const VmaAllocation* pAllocation = scene->instanceAllocation;
	
		const VkBuffer* const pBufferEnd = scene->instance + renderer->graphics.capacity;
		while (pBuffer != pBufferEnd)
			vmaDestroyBuffer(renderer->allocator, *pBuffer++, *pAllocation++);
	}

	mem_freeRange<VmaAllocation>(scene->instanceAllocation);
	mem_freeRange<VkBuffer>(scene->instance);

	delete scene;
}

int pushObjectInstance(const __Renderer renderer, const Scene _Scene, const ObjectInstance* const pObject) noexcept {
	const uint32_t instanceCount = pObject->instanceCount;
	
	if (!instanceCount)
		return -1;

	if (instanceCount > (_Scene->instanceCapacity - _Scene->instanceCount) || _Scene->indirectCommandCount >= _Scene->indirectCommandCapacity)
		return 1;

	const uint32_t firstInstance = _Scene->instanceCount;
	
	const InstanceData* const pInstanceDataSrcEnd = pObject->pInstances + pObject->instanceCount;
	
	const InstanceDataGPU* const* const ppInstanceDataGPUEnd = _Scene->instanceData + renderer->graphics.capacity;
	for (InstanceDataGPU* const* ppInstanceDataGPU{ _Scene->instanceData }; ppInstanceDataGPU != ppInstanceDataGPUEnd;) {
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

	const VkDrawIndexedIndirectCommand* const* const ppDrawDataEnd = _Scene->indirectData + renderer->graphics.capacity;
	for (VkDrawIndexedIndirectCommand* const* ppDrawData{ _Scene->indirectData }; ppDrawData != ppDrawDataEnd; ++ppDrawData)
		*ppDrawData[drawIndex] = command;

	_Scene->instanceCount += instanceCount;
	++_Scene->indirectCommandCount;

	return 0;
}

struct CameraDataGPU {
	CameraData external;
};

struct Camera_T {
	VkBuffer* buffer;
	VmaAllocation* allocation;
	CameraDataGPU** data;

	VkDescriptorPool descriptorPool;
	VkDescriptorSet* descriptorSet;
};

int createCamera(__Renderer renderer, const CameraCreateInfo* pCreateInfo, mem_stack* pScratch, Camera* pCamera) noexcept {
	const Camera camera = new(std::nothrow) Camera_T;

	if (!camera)
		return -1;

	camera->buffer = mem_allocateRange<VkBuffer>(renderer->graphics.capacity);

	if (!camera->buffer)
		goto failure_0;

	mem_nullifyRange<VkBuffer>(camera->buffer, renderer->graphics.capacity);

	camera->allocation = mem_allocateRange<VmaAllocation>(renderer->graphics.capacity);

	if (!camera->allocation)
		goto failure_1;

	camera->data = mem_allocateRange<CameraDataGPU*>(renderer->graphics.capacity);

	if (!camera->data)
		goto failure_2;

	{
		const size_t size = sizeof(CameraDataGPU) * pCreateInfo->bindings;

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

		VkBuffer* pBuffer = camera->buffer;
		VmaAllocation* pAllocation = camera->allocation;
		CameraDataGPU** ppData = camera->data;

		const VkBuffer* const pBufferEnd = camera->buffer + renderer->graphics.capacity;
		while (pBuffer != pBufferEnd) {
			if (vmaCreateBuffer(renderer->allocator, &createInfo, &allocationCreateInfo, pBuffer++, pAllocation++, &allocationInfo) != VK_SUCCESS)
				goto failure_3;

			*ppData++ = reinterpret_cast<CameraDataGPU*>(allocationInfo.pMappedData);
		}
	}

	{
		VkDescriptorPoolSize poolSize[1]{};
		poolSize[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSize[0].descriptorCount = renderer->graphics.capacity;

		VkDescriptorPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.maxSets = renderer->graphics.capacity;
		createInfo.poolSizeCount = 1u;
		createInfo.pPoolSizes = poolSize;

		if (vkCreateDescriptorPool(renderer->device, &createInfo, nullptr, &camera->descriptorPool) != VK_SUCCESS)
			goto failure_3;
	}

	camera->descriptorSet = mem_allocateRange<VkDescriptorSet>(renderer->graphics.capacity);

	if (!camera->descriptorSet)
		goto failure_4;

	{
		pScratch->frame();

		VkDescriptorSetLayout* setLayout = mem_stackAllocateRange<VkDescriptorSetLayout>(pScratch, renderer->graphics.capacity);

		if (!setLayout) {
			pScratch->restore();
			goto failure_4;
		}

		mem_assignRange<VkDescriptorSetLayout>(setLayout, renderer->graphics.capacity, renderer->pass.cameraSetLayout);

		VkDescriptorSetAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.pNext = nullptr;
		allocateInfo.descriptorPool = camera->descriptorPool;
		allocateInfo.descriptorSetCount = renderer->graphics.capacity;
		allocateInfo.pSetLayouts = setLayout;

		if (vkAllocateDescriptorSets(renderer->device, &allocateInfo, camera->descriptorSet) != VK_SUCCESS) {
			pScratch->restore();
			goto failure_4;
		}

		pScratch->restore();
	}

	{
		const size_t size = sizeof(CameraDataGPU) * pCreateInfo->bindings;

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

		const VkBuffer* pBuffer = camera->buffer;

		const VkDescriptorSet* const pDescriptorSetEnd = camera->descriptorSet + renderer->graphics.capacity;
		for (const VkDescriptorSet* pDescriptorSet{ camera->descriptorSet }; pDescriptorSet != pDescriptorSetEnd;) {
			bufferInfo.buffer = *pBuffer++;
			write.dstSet = *pDescriptorSet++;

			vkUpdateDescriptorSets(renderer->device, 1u, &write, 0, nullptr);
		}
	}

	*pCamera = camera;

	return 0;

failure_4:
	vkDestroyDescriptorPool(renderer->device, camera->descriptorPool, nullptr);

failure_3:
	{
		const VkBuffer* pBuffer = camera->buffer;
		const VmaAllocation* pAllocation = camera->allocation;

		const VkBuffer* const pBufferEnd = camera->buffer + renderer->graphics.capacity;
		while (pBuffer != pBufferEnd && *pBuffer)
			vmaDestroyBuffer(renderer->allocator, *pBuffer++, *pAllocation++);
	}

	mem_freeRange<CameraDataGPU*>(camera->data);
	
failure_2:
	mem_freeRange<VmaAllocation>(camera->allocation);

failure_1:
	mem_freeRange<VkBuffer>(camera->buffer);

failure_0:
	delete camera;

	return -1;
}

void destroyCamera(const __Renderer renderer, const Camera camera) noexcept {
	mem_freeRange<VkDescriptorSet>(camera->descriptorSet);
	vkDestroyDescriptorPool(renderer->device, camera->descriptorPool, nullptr);

	mem_freeRange<CameraDataGPU*>(camera->data);

	{
		const VkBuffer* pBuffer = camera->buffer;
		const VmaAllocation* pAllocation = camera->allocation;
	
		const VkBuffer* const pBufferEnd = camera->buffer + renderer->graphics.capacity;
		while (pBuffer != pBufferEnd)
			vmaDestroyBuffer(renderer->allocator, *pBuffer++, *pAllocation++);
	}

	mem_freeRange<VmaAllocation>(camera->allocation);
	mem_freeRange<VkBuffer>(camera->buffer);

	delete camera;
}

void updateCamera(const __Renderer renderer, const Camera _Camera, const CameraWrite* const pWrite) noexcept {
	const CameraData* const pDataSrcEnd = pWrite->pData + pWrite->count;
	
 	const CameraDataGPU* const* const ppDataEnd = _Camera->data + renderer->graphics.capacity;
	for (CameraDataGPU* const* ppData{ _Camera->data }; ppData != ppDataEnd;) {
		CameraDataGPU* pDataDst = *ppData++ + pWrite->firstCamera;
		
		for (const CameraData* pDataSrc{ pWrite->pData }; pDataSrc != pDataSrcEnd;)
			memcpy((void*)(reinterpret_cast<uint8_t*>(pDataDst++) + offsetof(CameraDataGPU, CameraDataGPU::external)), pDataSrc++, sizeof(CameraDataGPU));
	}
}

int beginFrame(__Renderer renderer) noexcept {
	uint32_t index;

	if (prepareRoundedSubmission(renderer->device, &renderer->graphics, &index))
		return -1;

	const VkSemaphore imageAvailable = renderer->imageAvailable[index];

	uint32_t imageIndex;

	switch (vkAcquireNextImageKHR(renderer->device, renderer->swapchain, UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &imageIndex)) {
	case VK_ERROR_OUT_OF_DATE_KHR:
		return 1;
	case VK_SUCCESS:
	case VK_SUBOPTIMAL_KHR:
		break;
	default:
		return -1;
	}

	VkFence imageFence = renderer->imageFence[imageIndex];

	if (imageFence && vkWaitForFences(renderer->device, 1u, &imageFence, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
		return -1;

	const VkCommandBuffer commandBuffer = renderer->graphics.thisCommandBuffer();

	if (vkResetCommandBuffer(commandBuffer, 0) != VK_SUCCESS)
		return -1;

	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pNext = nullptr;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			return -1;
	}

	renderer->acquire = imageIndex;

	return 0;
}

void render(const __Renderer renderer, const Collection _Collection, const Scene _Scene, const Texture texture, const Camera camera, const uint32_t cameraIndex) noexcept {
	const VkCommandBuffer commandBuffer = renderer->graphics.thisCommandBuffer();

	if (texture->state & RESOURCE_STATE_PENDING_TRANSFER_ACQUIRE) {
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcQueueFamilyIndex = renderer->queueFamily.transfer;
		barrier.dstQueueFamilyIndex = renderer->queueFamily.graphics;
		barrier.image = texture->image;
		barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1u, &barrier);

		texture->state &= ~RESOURCE_STATE_PENDING_TRANSFER_ACQUIRE;
	}

	const VkRect2D renderArea = { { 0, 0 }, renderer->extent };

	{
		VkRenderPassBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.pNext = nullptr;
		beginInfo.renderPass = renderer->pass.renderPass;
		beginInfo.framebuffer = renderer->framebuffer[renderer->acquire];
		beginInfo.renderArea = renderArea;
		beginInfo.clearValueCount = 2u;
		beginInfo.pClearValues = renderer->pass.clearValue;

		vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->pass.pipeline);

	{
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = renderer->extent.width;
		viewport.height = renderer->extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}
	
	vkCmdSetScissor(commandBuffer, 0, 1, &renderArea);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->pass.pipelineLayout, 0u, 1u, &camera->descriptorSet[renderer->graphics.index], 0u, nullptr);

	vkCmdPushConstants(commandBuffer, renderer->pass.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0u, sizeof(uint32_t), &cameraIndex);

	const VkDeviceSize offset[1] = { 0u };

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_Collection->vertex, offset);
	vkCmdBindIndexBuffer(commandBuffer, _Collection->index, 0u, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->pass.pipelineLayout, 1u, 1u, &_Scene->descriptorSet[renderer->graphics.index], 0u, nullptr);
	
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->pass.pipelineLayout, 2u, 1u, &texture->descriptorSet[renderer->graphics.index], 0u, nullptr);

	vkCmdDrawIndexedIndirect(commandBuffer, _Scene->indirect[renderer->graphics.index], 0, _Scene->indirectCommandCount, sizeof(VkDrawIndexedIndirectCommand));
	
	vkCmdEndRenderPass(commandBuffer);
}

int endFrame(const __Renderer renderer) noexcept {
	const VkCommandBuffer commandBuffer = renderer->graphics.thisCommandBuffer();

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		return -1;

	const VkFence fence = renderer->graphics.thisFence();

	if (vkResetFences(renderer->device, 1u, &fence) != VK_SUCCESS)
		return -1;

	const VkSemaphore imageAvailable = renderer->imageAvailable[renderer->graphics.index];
	const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	const VkSemaphore renderComplete = renderer->graphics.thisSemaphore();

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

		if (vkQueueSubmit(renderer->queue.graphics, 1u, &submitInfo, fence) != VK_SUCCESS)
			return -1;
	}

	renderer->imageFence[renderer->acquire] = fence;

	proceedRoundedSubmission(&renderer->graphics);

	{
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1u;
		presentInfo.pWaitSemaphores = &renderComplete;
		presentInfo.swapchainCount = 1u;
		presentInfo.pSwapchains = &renderer->swapchain;
		presentInfo.pResults = nullptr;
		presentInfo.pImageIndices = &renderer->acquire;

		switch (vkQueuePresentKHR(renderer->queue.present, &presentInfo)) {
		case VK_SUCCESS:
			break;
		case VK_SUBOPTIMAL_KHR:
		case VK_ERROR_OUT_OF_DATE_KHR:
			return 1;
		default:
			return -1;
		}
	}

	return 0;
}
