#include "presentation.h"

#include "Signboard/Core/Math/search.h"
#include "Signboard/Assets/io/ResourceLoaders/file_loader.h"
#include <array>
#include <vector>

namespace rndr {

	constexpr uint32_t MIN_SWAPCHAIN_IMAGE_COUNT = 2;
	constexpr uint32_t MAX_CONCURRENT_PRESENTATION_JOB_COUNT = MIN_SWAPCHAIN_IMAGE_COUNT;

	constexpr std::array preferredSurfaceFormats = {
		VkSurfaceFormatKHR{ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
		VkSurfaceFormatKHR{ VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
		VkSurfaceFormatKHR{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
		VkSurfaceFormatKHR{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }
	};

	struct SurfaceFormatEq {
		constexpr bool operator()(const VkSurfaceFormatKHR& a, const VkSurfaceFormatKHR& b) const noexcept {
			return a.format == b.format && a.colorSpace == b.colorSpace;
		}
	};

	constexpr std::array preferredPresentModes = {
		VK_PRESENT_MODE_MAILBOX_KHR,
		VK_PRESENT_MODE_FIFO_KHR ,
		VK_PRESENT_MODE_IMMEDIATE_KHR
	};

	VkResult presentationStage::root(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
		VkResult result;

		VkSurfaceFormatKHR surfaceFormat;

		{
			uint32_t supportedFormatCount = 0;
			result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &supportedFormatCount, nullptr);
			if (result != VK_SUCCESS)
				return result;

			std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &supportedFormatCount, supportedFormats.data());

			size_t index = 0;
			std::findEarly_basic<VkSurfaceFormatKHR, SurfaceFormatEq>(preferredSurfaceFormats.data(), preferredSurfaceFormats.size(), supportedFormats.data(), supportedFormats.size(), &index);

			surfaceFormat = supportedFormats[index];
		}

		VkPresentModeKHR surfacePresentMode;

		{
			uint32_t supportedPresentModeCount = 0;
			result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &supportedPresentModeCount, nullptr);
			if (result != VK_SUCCESS)
				return result;

			std::vector<VkPresentModeKHR> supportedPresentModes(supportedPresentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &supportedPresentModeCount, supportedPresentModes.data());

			size_t index = 0;
			std::findEarly_basic(preferredPresentModes.data(), preferredPresentModes.size(), supportedPresentModes.data(), supportedPresentModes.size(), &index);

			surfacePresentMode = supportedPresentModes[index];
		}
		

		result = configurePasses(device);
		if (result != VK_SUCCESS)
			return result;

		result = configureBindings(device);
		if (result != VK_SUCCESS)
			return result;

		result = configureRendering(device);
		if (result != VK_SUCCESS)
			return result;

		result = configureExecution(device);
		if (result != VK_SUCCESS)
			return result;

		if (!renderJobs.empty()) {
			size_t renderJobCount = renderJobs.size();
			for (size_t i = 0; i < renderJobCount; ++i) {
				vkDestroyFence(r_device, renderJobs[i].inFlight, nullptr);
				
				vkDestroySemaphore(r_device, renderJobs[i].imageAvailable, nullptr);
				vkDestroySemaphore(r_device, renderJobs[i].renderComplete, nullptr);
			}
		}

		renderJobs.clear();

		r_device = device;
		r_physicalDevice = physicalDevice;
		r_surface = surface;

		return VK_SUCCESS;
	}

	VkResult presentationStage::configurePasses(VkDevice device) noexcept {
		VkResult result;

		VkRenderPass renderPass;

		{
			VkAttachmentDescription output{};
			output.format = configuration.surfaceFormat.format;
			output.samples = VK_SAMPLE_COUNT_1_BIT;
			output.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			output.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			output.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			output.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			output.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			output.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference outputReference{};
			outputReference.attachment = 0;
			outputReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass{};
			subpass.flags = 0;
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.inputAttachmentCount = 0;
			subpass.pInputAttachments = nullptr;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &outputReference;
			subpass.pResolveAttachments = nullptr;
			subpass.pDepthStencilAttachment = nullptr;
			subpass.preserveAttachmentCount = 0;
			subpass.pPreserveAttachments = nullptr;

			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependency.dependencyFlags = 0;

			VkRenderPassCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.attachmentCount = 1;
			createInfo.pAttachments = &output;
			createInfo.subpassCount = 1;
			createInfo.pSubpasses = &subpass;
			createInfo.dependencyCount = 1;
			createInfo.pDependencies = &dependency;

			result = vkCreateRenderPass(device, &createInfo, nullptr, &renderPass);
			if (result != VK_SUCCESS)
				return result;
		}

		if (passes.compositionPass)
			vkDestroyRenderPass(r_device, passes.compositionPass, nullptr);

		passes.compositionPass = renderPass;

		return VK_SUCCESS;
	}

	VkResult presentationStage::configureBindings(VkDevice device) noexcept {
		VkResult result;
		
		VkSampler sampler;

		{
			VkSamplerCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.magFilter = VK_FILTER_LINEAR;
			createInfo.minFilter = VK_FILTER_LINEAR;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			createInfo.mipLodBias = 1.0f;
			createInfo.anisotropyEnable = VK_FALSE;
			createInfo.maxAnisotropy = 1.0f;
			createInfo.compareEnable = VK_FALSE;
			createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
			createInfo.minLod = 0.0f;
			createInfo.maxLod = VK_LOD_CLAMP_NONE;
			createInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
			createInfo.unnormalizedCoordinates = VK_FALSE;

			result = vkCreateSampler(device, &createInfo, nullptr, &sampler);
			if (result != VK_SUCCESS)
			return result;
		}

		VkDescriptorSetLayout descriptorSetLayout;

		{
			VkDescriptorSetLayoutBinding image{};
			image.binding = 0;
			image.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			image.descriptorCount = 1;
			image.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			image.pImmutableSamplers = &sampler;

			VkDescriptorSetLayoutCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.bindingCount = 1;
			createInfo.pBindings = &image;

			result = vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &descriptorSetLayout);
			if (result != VK_SUCCESS)
				vkDestroySampler(device, sampler, nullptr);

				return result;
		}

		VkDescriptorPool descriptorPool;

		{
			VkDescriptorPoolSize poolSize{};
			poolSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			poolSize.descriptorCount = MAX_CONCURRENT_PRESENTATION_JOB_COUNT;

			VkDescriptorPoolCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.maxSets = MAX_CONCURRENT_PRESENTATION_JOB_COUNT;
			createInfo.poolSizeCount = 1;
			createInfo.pPoolSizes = &poolSize;

			result = vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool);
			if (result != VK_SUCCESS) {
				vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
				vkDestroySampler(device, sampler, nullptr);

				return result;
			}
		}

		std::vector<VkDescriptorSet> descriptorSets(MAX_CONCURRENT_PRESENTATION_JOB_COUNT);

		{
			VkDescriptorSetAllocateInfo allocateInfo{};
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.descriptorPool = bindings.descriptorPool;
			allocateInfo.descriptorSetCount = MAX_CONCURRENT_PRESENTATION_JOB_COUNT;
			allocateInfo.pSetLayouts = &bindings.descriptorSetLayout;

			result = vkAllocateDescriptorSets(device, &allocateInfo, descriptorSets.data());
			if (result != VK_SUCCESS) {
				vkDestroyDescriptorPool(device, descriptorPool, nullptr);
				vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
				vkDestroySampler(device, sampler, nullptr);

				return result;
			}
		}

		if (bindings.descriptorPool)
			vkDestroyDescriptorPool(r_device, bindings.descriptorPool, nullptr);
		
		bindings.descriptorPool = descriptorPool;

		bindings.descriptorSets = std::move(descriptorSets);
		
		if (bindings.descriptorSetLayout)
			vkDestroyDescriptorSetLayout(r_device, bindings.descriptorSetLayout, nullptr);
		
		bindings.descriptorSetLayout = descriptorSetLayout;
		
		if (bindings.sampler)
			vkDestroySampler(r_device, bindings.sampler, nullptr);

		bindings.sampler = sampler;

		return VK_SUCCESS;
	}

	VkResult presentationStage::configureRendering(VkDevice device) noexcept {
		VkResult result;

		VkPipelineLayout pipelineLayout;

		{
			VkPipelineLayoutCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.setLayoutCount = 1;
			createInfo.pSetLayouts = &bindings.descriptorSetLayout;
			createInfo.pushConstantRangeCount = 0;
			createInfo.pPushConstantRanges = nullptr;

			result = vkCreatePipelineLayout(device, &createInfo, nullptr, &pipelineLayout);
			if (result != VK_SUCCESS)
				return result;
		}

		VkPipeline pipeline;

		{
			contextual_pool<rhi::shaderModule> shaderModules{ device };
			std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

			{
				size_t vertexModuleIndex = 0;
				size_t fragmentModuleIndex = 0;

				{
					io::loader::file_loader loader;

					VkShaderModuleCreateInfo createInfo{};
					createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
					createInfo.pNext = nullptr;
					createInfo.flags = 0;

					std::vector<uint32_t> spirv = loader.load_SPIRV("shaders/base.vert");

					createInfo.codeSize = spirv.size() * sizeof(uint32_t);
					createInfo.pCode = spirv.data();

					result = shaderModules.create(&createInfo, &vertexModuleIndex);
					if (result != VK_SUCCESS)
						return result;

					spirv = loader.load_SPIRV("shaders/base.frag");

					createInfo.codeSize = spirv.size() * sizeof(uint32_t);
					createInfo.pCode = spirv.data();

					result = shaderModules.create(&createInfo, &fragmentModuleIndex);
					if (result != VK_SUCCESS)
						return result;
				}

				shaderStages.reserve(2);

				VkPipelineShaderStageCreateInfo stageInfo{};
				stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				stageInfo.pNext = nullptr;
				stageInfo.flags = 0;
				stageInfo.pName = "main";

				stageInfo.module = shaderModules[vertexModuleIndex];
				stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

				shaderStages.push_back(stageInfo);

				stageInfo.module = shaderModules[fragmentModuleIndex];
				stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

				shaderStages.push_back(stageInfo);
			}

			VkPipelineVertexInputStateCreateInfo vertexInputState{};
			vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputState.pNext = nullptr;
			vertexInputState.flags = 0;
			vertexInputState.vertexBindingDescriptionCount = 0;
			vertexInputState.pVertexBindingDescriptions = nullptr;
			vertexInputState.vertexAttributeDescriptionCount = 0;
			vertexInputState.pVertexAttributeDescriptions = nullptr;

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
			createInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
			createInfo.pStages = shaderStages.data();
			createInfo.pVertexInputState = &vertexInputState;
			createInfo.pInputAssemblyState = &inputAssemblyState;
			createInfo.pTessellationState = nullptr;
			createInfo.pViewportState = &viewportState;
			createInfo.pRasterizationState = &rasterizationState;
			createInfo.pMultisampleState = &multisampleState;
			createInfo.pDepthStencilState = &depthStencilState;
			createInfo.pColorBlendState = &colorBlendState;
			createInfo.pDynamicState = &dynamicState;
			createInfo.layout = pipelineLayout;
			createInfo.renderPass = passes.compositionPass;
			createInfo.subpass = 0;
			createInfo.basePipelineHandle = VK_NULL_HANDLE;
			createInfo.basePipelineIndex = 0;

			result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);
			if (result != VK_SUCCESS) {
				vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

				return result;
			}
		}

		if (rendering.pipelineLayout)
			vkDestroyPipelineLayout(r_device, rendering.pipelineLayout, nullptr);

		rendering.pipelineLayout = pipelineLayout;

		if (rendering.pipeline)
			vkDestroyPipeline(r_device, rendering.pipeline, nullptr);

		rendering.pipeline = pipeline;

		return VK_SUCCESS;
	}

	VkResult presentationStage::configureExecution(VkDevice device) noexcept {
		VkResult result;

		VkCommandPool commandPool;

		{
			VkCommandPoolCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			createInfo.queueFamilyIndex = execution.graphicsFamily;

			result = vkCreateCommandPool(device, &createInfo, nullptr, &commandPool);
			if (result != VK_SUCCESS)
				return result;
		}

		std::vector<VkCommandBuffer> commandBuffers(MAX_CONCURRENT_PRESENTATION_JOB_COUNT);

		{
			VkCommandBufferAllocateInfo allocateInfo{};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = execution.commandPool;
			allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = MAX_CONCURRENT_PRESENTATION_JOB_COUNT;

			result = vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data());
			if (result != VK_SUCCESS) {
				vkDestroyCommandPool(device, commandPool, nullptr);

				return result;
			}
		}

		if (execution.commandPool)
			vkDestroyCommandPool(r_device, execution.commandPool, nullptr);

		execution.commandPool = commandPool;

		execution.commandBuffers = std::move(commandBuffers);

		return VK_SUCCESS;
	}

	VkResult presentationStage::configurePresentation() noexcept {
		VkResult result;

		VkExtent2D extent;

		VkSwapchainKHR swapchain;

		{
			VkSurfaceCapabilitiesKHR surfaceCapabilities{};

			result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(r_physicalDevice, r_surface, &surfaceCapabilities);
			if (result != VK_SUCCESS)
				return result;

			extent = surfaceCapabilities.currentExtent;

			VkSwapchainCreateInfoKHR createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.surface = r_surface;
			createInfo.minImageCount = surfaceCapabilities.maxImageCount ? std::clamp(MIN_SWAPCHAIN_IMAGE_COUNT, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount) : std::max(MIN_SWAPCHAIN_IMAGE_COUNT, surfaceCapabilities.minImageCount);
			createInfo.imageFormat = configuration.surfaceFormat.format;
			createInfo.imageColorSpace = configuration.surfaceFormat.colorSpace;
			createInfo.imageExtent = surfaceCapabilities.currentExtent;
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
			createInfo.preTransform = surfaceCapabilities.currentTransform;
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.presentMode = configuration.surfacePresentMode;
			createInfo.clipped = VK_TRUE;
			createInfo.oldSwapchain = presentation.swapchain;

			result = vkCreateSwapchainKHR(r_device, &createInfo, nullptr, &swapchain);
			if (result != VK_SUCCESS)
				return result;

			extent = surfaceCapabilities.currentExtent;
		}

		uint32_t imageCount;

		std::vector<VkImageView> imageViews;

		{
			uint32_t swapchainImageCount = 0;
			vkGetSwapchainImagesKHR(r_device, swapchain, &swapchainImageCount, nullptr);

			std::vector<VkImage> swapchainImages(swapchainImageCount);
			vkGetSwapchainImagesKHR(r_device, swapchain, &swapchainImageCount, swapchainImages.data());

			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.image = VK_NULL_HANDLE;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = configuration.surfaceFormat.format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			imageViews.resize(swapchainImageCount);

			for (uint32_t i = 0; i < swapchainImageCount; ++i) {
				createInfo.image = swapchainImages[i];
				result = vkCreateImageView(r_device, &createInfo, nullptr, &imageViews[i]);
				if (result != VK_SUCCESS) {
					for (uint32_t j = 0; j < i; ++j) {
						vkDestroyImageView(r_device, imageViews[j], nullptr);
					}
					vkDestroySwapchainKHR(r_device, swapchain, nullptr);

					return result;
				}
			}

			imageCount = swapchainImageCount;
		}

		std::vector<VkFramebuffer> framebuffers;

		{
			VkFramebufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.renderPass = passes.compositionPass;
			createInfo.attachmentCount = 1;
			createInfo.pAttachments = nullptr;
			createInfo.width = extent.width;
			createInfo.height = extent.height;
			createInfo.layers = 1;

			framebuffers.resize(imageCount);

			for (uint32_t i = 0; i < imageCount; ++i) {
				createInfo.pAttachments = &imageViews[i];

				result = vkCreateFramebuffer(r_device, &createInfo, nullptr, &framebuffers[i]);
				if (result != VK_SUCCESS) {
					for (uint32_t j = 0; j < i; ++j) {
						vkDestroyFramebuffer(r_device, framebuffers[j], nullptr);
					}
					for (uint32_t j = 0; j < imageCount; ++j) {
						vkDestroyImageView(r_device, imageViews[j], nullptr);
					}
					vkDestroySwapchainKHR(r_device, swapchain, nullptr);

					return result;
				}
			}
		}

		size_t presentJobCount = presentJobs.size();
		for (size_t i = 0; i < presentJobCount; ++i) {
			VkFence fence = presentJobs[i];
			if (!fence)
				continue;

			result = vkWaitForFences(r_device, 1, &fence, VK_TRUE, UINT64_MAX);
			if (result != VK_SUCCESS)
				return result;
		}

		presentJobs.assign(imageCount, VK_NULL_HANDLE);

		size_t framebufferCount = presentation.framebuffers.size();

		for (size_t i = 0; i < framebufferCount; ++i) {
			vkDestroyFramebuffer(r_device, presentation.framebuffers[i], nullptr);
		}

		presentation.framebuffers = std::move(framebuffers);

		presentation.extent = extent;

		size_t imageViewCount = presentation.swapchainImageViews.size();

		for (size_t i = 0; i < imageViewCount; ++i) {
			vkDestroyImageView(r_device, presentation.swapchainImageViews[i], nullptr);
		}

		presentation.swapchainImageViews = std::move(imageViews);

		presentation.imageCount = imageCount;

		if (presentation.swapchain)
			vkDestroySwapchainKHR(r_device, presentation.swapchain, nullptr);

		presentation.swapchain = swapchain;

		return VK_SUCCESS;
	}

	VkResult presentationStage::stagePresentation(VkImageView view) {
		VkResult result;

		RenderJob* pJob = nullptr;

		if (!renderJobs.empty())
			pJob = &renderJobs[jobHint];

		if (pJob) {
			result = vkGetFenceStatus(r_device, pJob->inFlight);

			size_t jobCount = renderJobs.size();

			switch (result) {
			case VK_SUCCESS:
				bindings.descriptorSets.emplace_back(pJob->descriptorSet);
				execution.commandBuffers.emplace_back(pJob->commandBuffer);

				jobHint = (jobHint + 1) % jobCount;
				break;

			case VK_NOT_READY:
				if (jobCount < MAX_CONCURRENT_PRESENTATION_JOB_COUNT) {
					pJob = nullptr;
					
					break;
				}
				
				result = vkWaitForFences(r_device, 1, &pJob->inFlight, VK_TRUE, UINT16_MAX);
				if (result != VK_SUCCESS)
					return result;
				
				break;

			default:
				return result;
			}
		}

		if (!pJob) {
			RenderJob job{};

			{
				VkFenceCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;

				result = vkCreateFence(r_device, &createInfo, nullptr, &job.inFlight);
				if (result != VK_SUCCESS)
					return result;
			}

			{
				VkSemaphoreCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;

				result = vkCreateSemaphore(r_device, &createInfo, nullptr, &job.imageAvailable);
				if (result != VK_SUCCESS) {
					vkDestroyFence(r_device, job.inFlight, nullptr);
					return result;
				}

				result = vkCreateSemaphore(r_device, &createInfo, nullptr, &job.renderComplete);
				if (result != VK_SUCCESS) {
					vkDestroySemaphore(r_device, job.imageAvailable, nullptr);
					vkDestroyFence(r_device, job.inFlight, nullptr);

					return result;
				}
			}

			renderJobs.emplace_back(job);
			pJob = &renderJobs.back();
		}

		uint32_t availableImageIndex = 0;

		{
			result = vkAcquireNextImageKHR(r_device, presentation.swapchain, UINT64_MAX, pJob->imageAvailable, VK_NULL_HANDLE, &availableImageIndex);
			if (result != VK_SUCCESS)
				return result;

			VkFence imageFence = presentJobs[availableImageIndex];
			if (imageFence) {
				result = vkWaitForFences(r_device, 1, &imageFence, VK_TRUE, UINT32_MAX);
				if (result != VK_SUCCESS)
					return result;
			}
		}
		
		VkDescriptorSet descriptorSet = bindings.descriptorSets.back();

		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = view;
			imageInfo.sampler = VK_NULL_HANDLE;

			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.pNext = nullptr;
			write.dstSet = descriptorSet;
			write.dstBinding = 0;
			write.dstArrayElement = 0;
			write.descriptorCount = 1;
			write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			write.pImageInfo = &imageInfo;
			write.pBufferInfo = nullptr;
			write.pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(r_device, 1, &write, 0, nullptr);
		}

		VkCommandBuffer commandBuffer = execution.commandBuffers.back();

		{
			vkResetCommandBuffer(commandBuffer, 0);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = nullptr;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = nullptr;

			result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
			if (result != VK_SUCCESS) {
				return result;
			}
		}
		
		{
			VkRect2D renderArea{};
			renderArea.offset = { 0, 0 };
			renderArea.extent = presentation.extent;

			VkRenderPassBeginInfo pass{};
			pass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			pass.pNext = nullptr;
			pass.renderPass = passes.compositionPass;
			pass.framebuffer = presentation.framebuffers[availableImageIndex];
			pass.renderArea = renderArea;
			pass.clearValueCount = 0;
			pass.pClearValues = nullptr;

			vkCmdBeginRenderPass(commandBuffer, &pass, VK_SUBPASS_CONTENTS_INLINE);
		}
		
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rendering.pipeline);

		{
			VkViewport viewport{};
			viewport.x = 0;
			viewport.y = 0;
			viewport.width = presentation.extent.width;
			viewport.height = presentation.extent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

			VkRect2D scissors{};
			scissors.offset = { 0,0 };
			scissors.extent = presentation.extent;

			vkCmdSetScissor(commandBuffer, 0, 1, &scissors);
		}

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rendering.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

		vkCmdDraw(commandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		result = vkEndCommandBuffer(commandBuffer);
		if (result != VK_SUCCESS)
			return result;

		{
			VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pNext = nullptr;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &pJob->imageAvailable;
			submitInfo.pWaitDstStageMask = &waitStage;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &pJob->renderComplete;

			vkResetFences(r_device, 1, &pJob->inFlight);

			result = vkQueueSubmit(execution.graphicsQueue, 1, &submitInfo, pJob->inFlight);
			if (result != VK_SUCCESS) {
				return result;
			}
		}

		presentJobs[availableImageIndex] = pJob->inFlight;

		pJob->descriptorSet = descriptorSet;
		bindings.descriptorSets.pop_back();

		pJob->commandBuffer = commandBuffer;
		execution.commandBuffers.pop_back();

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &pJob->renderComplete;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &presentation.swapchain;
		presentInfo.pImageIndices = &availableImageIndex;
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(presentation.presentQueue, &presentInfo);
		if (result != VK_SUCCESS)
			return result;

		return VK_SUCCESS;
	}

	presentationStage::~presentationStage() noexcept {

	}

}

//{
//	uint32_t presentationFamilyIndex = UINT32_MAX;
//
//	uint32_t familyCount = 0;
//	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);
//
//	std::vector<VkQueueFamilyProperties> queueFamilyProperties(familyCount);
//	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, queueFamilyProperties.data());
//
//	std::vector<VkBool32> presentationSupport(familyCount, VK_FALSE);
//	for (uint32_t i = 0; i < familyCount; ++i) {
//		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentationSupport[i]);
//	}
//
//	if (selectedGraphicsFamilyIndex != UINT32_MAX && presentationSupport[selectedGraphicsFamilyIndex]) {
//		presentationFamilyIndex = selectedGraphicsFamilyIndex;
//	}
//
//	if (presentationFamilyIndex == UINT32_MAX) {
//		for (uint32_t i = 0; i < familyCount; ++i) {
//			if (!presentationSupport[i])
//				continue;
//
//			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
//				presentationFamilyIndex = i;
//				break;
//			}
//
//			if (presentationFamilyIndex == UINT32_MAX) {
//				presentationFamilyIndex = i;
//			}
//		}
//	}
//
//	if (presentationFamilyIndex == UINT32_MAX)
//		return VK_ERROR_FEATURE_NOT_PRESENT;
//
//	*presentFamilyIndex = presentationFamilyIndex;
//
//	presentationFamily = presentationFamilyIndex;
//		}