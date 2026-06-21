#include "presentation.h"

#include "Signboard/Core/Math/search.h"
#include "Signboard/Assets/io/ResourceLoaders/file_loader.h"
#include <array>
#include <vector>

namespace rndr {

	VkResult presentationStage::root(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const PresentationStageCreateInfo* pCreateInfo) {
		VkResult result;

		VkSurfaceFormatKHR surfaceFormat;

		{
			uint32_t supportedFormatCount = 0;
			result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &supportedFormatCount, nullptr);
			if (result != VK_SUCCESS)
				return result;

			std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &supportedFormatCount, supportedFormats.data());

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

			constexpr std::array preferredPresentModes = {
				VK_PRESENT_MODE_MAILBOX_KHR,
				VK_PRESENT_MODE_FIFO_KHR ,
				VK_PRESENT_MODE_IMMEDIATE_KHR
			};

			size_t index = 0;
			std::findEarly_basic(preferredPresentModes.data(), preferredPresentModes.size(), supportedPresentModes.data(), supportedPresentModes.size(), &index);

			surfacePresentMode = supportedPresentModes[index];
		}
		
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
			if (result != VK_SUCCESS) {
				vkDestroyRenderPass(device, renderPass, nullptr);

				return result;
			}
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
			if (result != VK_SUCCESS) {
				vkDestroySampler(device, sampler, nullptr);
				vkDestroyRenderPass(device, renderPass, nullptr);
				
				return result;
			}
		}

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
			if (result != VK_SUCCESS) {
				

				return result;
			}
		}

		VkPipeline pipeline;

		{
			VkShaderModule vertexModule;

			{
				std::vector<uint32_t> spirv = io::loader::file_loader::load_SPIRV("shaders/base.vert");

				VkShaderModuleCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				createInfo.codeSize = spirv.size() * sizeof(uint32_t);
				createInfo.pCode = spirv.data();

				result = vkCreateShaderModule(device, &createInfo, nullptr, &vertexModule);
				if (result != VK_SUCCESS) {
					vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
					vkDestroySampler(device, sampler, nullptr);
					vkDestroyRenderPass(device, renderPass, nullptr);

					return result;
				}
			}
			
			VkShaderModule fragmentModule;

			{
				std::vector<uint32_t> spirv = io::loader::file_loader::load_SPIRV("shaders/base.frag");

				VkShaderModuleCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				createInfo.codeSize = spirv.size() * sizeof(uint32_t);
				createInfo.pCode = spirv.data();

				result = vkCreateShaderModule(device, &createInfo, nullptr, &fragmentModule);
				if (result != VK_SUCCESS) {
					vkDestroyShaderModule(device, vertexModule, nullptr);
					vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
					vkDestroySampler(device, sampler, nullptr);
					vkDestroyRenderPass(device, renderPass, nullptr);

					return result;
				}
			}

			std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

			shaderStages.reserve(2);

			{
				VkPipelineShaderStageCreateInfo stageInfo{};
				stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				stageInfo.pNext = nullptr;
				stageInfo.flags = 0;
				stageInfo.pName = "main";
				stageInfo.module = vertexModule;
				stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

				shaderStages.push_back(stageInfo);
			}

			{
				VkPipelineShaderStageCreateInfo stageInfo{};
				stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				stageInfo.pNext = nullptr;
				stageInfo.flags = 0;
				stageInfo.pName = "main";
				stageInfo.module = fragmentModule;
				stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

				shaderStages.push_back(stageInfo);
			}

			contextual_pool<rhi::shaderModule> shaderModules{ device };

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

			vkDestroyShaderModule(device, fragmentModule, nullptr);
			vkDestroyShaderModule(device, vertexModule, nullptr);

			if (result != VK_SUCCESS) {
				vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
				vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
				vkDestroySampler(device, sampler, nullptr);
				vkDestroyRenderPass(device, renderPass, nullptr);

				return result;
			}
		}

		VkCommandPool commandPool;

		{
			VkCommandPoolCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			createInfo.queueFamilyIndex = pCreateInfo->graphicsFamilyIndex;

			result = vkCreateCommandPool(device, &createInfo, nullptr, &commandPool);
			if (result != VK_SUCCESS) {
				vkDestroyPipeline(r_device, pipeline, nullptr);
				vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
				vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
				vkDestroySampler(device, sampler, nullptr);
				vkDestroyRenderPass(device, renderPass, nullptr);

				return result;
			}
		}

		if (execution.commandPool)
			vkDestroyCommandPool(r_device, execution.commandPool, nullptr);

		execution.commandPool = commandPool;

		execution.graphicsFamily = pCreateInfo->graphicsFamilyIndex;
		execution.graphicsQueue = pCreateInfo->graphicsQueue;

		if (rendering.pipeline)
			vkDestroyPipeline(r_device, rendering.pipeline, nullptr);

		rendering.pipeline = pipeline;

		if (rendering.pipelineLayout)
			vkDestroyPipelineLayout(r_device, rendering.pipelineLayout, nullptr);

		rendering.pipelineLayout = pipelineLayout;

		if (bindings.descriptorSetLayout)
			vkDestroyDescriptorSetLayout(r_device, bindings.descriptorSetLayout, nullptr);

		bindings.descriptorSetLayout = descriptorSetLayout;

		if (bindings.sampler)
			vkDestroySampler(r_device, bindings.sampler, nullptr);

		bindings.sampler = sampler;

		if (passes.compositionPass)
			vkDestroyRenderPass(r_device, passes.compositionPass, nullptr);

		passes.compositionPass = renderPass;

		if (!jobs.empty()) {
			const size_t jobCount = jobs.size();
			for (size_t i = 0; i < jobCount; ++i) {
				VkFence fence = jobs[i].inFlight;
				if (!fence)
					continue;

				vkWaitForFences(r_device, 1, &fence, VK_TRUE, UINT64_MAX);

				vkDestroyFence(r_device, fence, nullptr);
				vkDestroySemaphore(r_device, jobs[i].imageAvailable, nullptr);
				vkDestroySemaphore(r_device, jobs[i].renderComplete, nullptr);
			}
			
			jobs.clear();
		}

		presentation.presentFamily = pCreateInfo->presentFamilyIndex;
		presentation.presentQueue = pCreateInfo->presentQueue;

		r_device = device;
		r_physicalDevice = physicalDevice;
		r_surface = surface;

		return VK_SUCCESS;
	}

	VkResult presentationStage::configurePresentation(uint32_t minImageCount) noexcept {
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
			createInfo.minImageCount = surfaceCapabilities.maxImageCount ? std::clamp(minImageCount, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount) : std::max(minImageCount, surfaceCapabilities.minImageCount);
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

		const size_t availableJobCount = jobs.size();

		if (availableJobCount < imageCount) {
			const size_t requiredRenderJobCount = imageCount - availableJobCount;

			for (size_t i = 0; i < requiredRenderJobCount; ++i) {
				presentJob job;

				{
					VkFenceCreateInfo createInfo{};
					createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
					createInfo.pNext = nullptr;
					createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

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

				{
					VkCommandBufferAllocateInfo allocateInfo{};
					allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
					allocateInfo.pNext = nullptr;
					allocateInfo.commandPool = execution.commandPool;
					allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
					allocateInfo.commandBufferCount = 1;

					result = vkAllocateCommandBuffers(r_device, &allocateInfo, &job.commandBuffer);
					if (result != VK_SUCCESS) {
						return result;
					}
				}

				jobs.emplace_back(job);
			}

		}

		{
			const size_t presentJobCount = jobs.size();
			for (size_t i = 0; i < presentJobCount; ++i) {
				VkFence fence = jobs[i].inFlight;
				if (!fence)
					continue;

				result = vkWaitForFences(r_device, 1, &fence, VK_TRUE, UINT64_MAX);
				if (result != VK_SUCCESS)
					return result;
			}
		}

		const size_t framebufferCount = presentation.framebuffers.size();

		for (size_t i = 0; i < framebufferCount; ++i) {
			vkDestroyFramebuffer(r_device, presentation.framebuffers[i], nullptr);
		}

		presentation.framebuffers = std::move(framebuffers);

		presentation.extent = extent;

		const size_t imageViewCount = presentation.swapchainImageViews.size();

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

	VkResult presentationStage::presentationImagePool(const VkImageView* pImageViews, uint32_t count) {
		VkResult result;
		
		const size_t availableDescriptorSetCount = bindings.descriptorSets.size();

		if (availableDescriptorSetCount < count) {
			VkDescriptorPool descriptorPool;

			{
				VkDescriptorPoolSize poolSize{};
				poolSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				poolSize.descriptorCount = count;

				VkDescriptorPoolCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				createInfo.maxSets = count;
				createInfo.poolSizeCount = 1;
				createInfo.pPoolSizes = &poolSize;

				result = vkCreateDescriptorPool(r_device, &createInfo, nullptr, &descriptorPool);
				if (result != VK_SUCCESS) {
					return result;
				}
			}

			std::vector<VkDescriptorSet> descriptorSets(count);

			{
				VkDescriptorSetAllocateInfo allocateInfo{};
				allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocateInfo.pNext = nullptr;
				allocateInfo.descriptorPool = bindings.descriptorPool;
				allocateInfo.descriptorSetCount = count;
				allocateInfo.pSetLayouts = &bindings.descriptorSetLayout;

				result = vkAllocateDescriptorSets(r_device, &allocateInfo, descriptorSets.data());
				if (result != VK_SUCCESS) {
					return result;
				}
			}

			if (bindings.descriptorPool)
				vkDestroyDescriptorPool(r_device, bindings.descriptorPool, nullptr);

			bindings.descriptorPool = descriptorPool;

			bindings.descriptorSets = std::move(descriptorSets);
		}

		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = VK_NULL_HANDLE;
			imageInfo.sampler = VK_NULL_HANDLE;

			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.pNext = nullptr;
			write.dstSet = VK_NULL_HANDLE;
			write.dstBinding = 0;
			write.dstArrayElement = 0;
			write.descriptorCount = 1;
			write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			write.pImageInfo = &imageInfo;
			write.pBufferInfo = nullptr;
			write.pTexelBufferView = nullptr;

			for (uint32_t i = 0; i < count; ++i) {
				imageInfo.imageView = pImageViews[i];
				write.dstSet = bindings.descriptorSets[i];

				vkUpdateDescriptorSets(r_device, 1, &write, 0, nullptr);
			}
		}

		return VK_SUCCESS;
	}

	VkResult presentationStage::stagePresentation(uint32_t imageIndex, VkSemaphore waitSemaphore) {
		VkResult result;

		presentJob& job = jobs[jobHint];

		result = vkWaitForFences(r_device, 1, &job.inFlight, VK_TRUE, UINT32_MAX);
		if (result != VK_SUCCESS)
			return result;

		jobHint = (jobHint + 1) % jobs.size();

		uint32_t availableImageIndex = 0;

		result = vkAcquireNextImageKHR(r_device, presentation.swapchain, UINT64_MAX, job.imageAvailable, VK_NULL_HANDLE, &availableImageIndex);
		if (result != VK_SUCCESS)
			return result;

		VkCommandBuffer commandBuffer = job.commandBuffer;

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

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rendering.pipelineLayout, 0, 1, &bindings.descriptorSets[imageIndex], 0, nullptr);

		job.descriptorSet = bindings.descriptorSets[imageIndex];

		vkCmdDraw(commandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		result = vkEndCommandBuffer(commandBuffer);
		if (result != VK_SUCCESS)
			return result;

		{
			VkSemaphore waitSemaphores[2] = {job.imageAvailable, waitSemaphore};

			VkPipelineStageFlags waitStages[2] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pNext = nullptr;
			submitInfo.waitSemaphoreCount = 2;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &job.renderComplete;

			vkResetFences(r_device, 1, &job.inFlight);

			result = vkQueueSubmit(execution.graphicsQueue, 1, &submitInfo, job.inFlight);
			if (result != VK_SUCCESS) {
				return result;
			}
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &job.renderComplete;
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
		{
			const size_t jobCount = jobs.size();
			for (size_t i = 0; i < jobCount; ++i) {
				VkFence fence = jobs[i].inFlight;
				if (!fence)
					continue;

				vkWaitForFences(r_device, 1, &fence, VK_TRUE, UINT64_MAX);

				vkDestroyFence(r_device, fence, nullptr);
				vkDestroySemaphore(r_device, jobs[i].imageAvailable, nullptr);
				vkDestroySemaphore(r_device, jobs[i].renderComplete, nullptr);
			}
		}
		
		if (presentation.swapchain) {
			const size_t framebufferCount = presentation.framebuffers.size();
			for (size_t i = 0; i < framebufferCount; ++i) {
				vkDestroyFramebuffer(r_device, presentation.framebuffers[i], nullptr);
			}

			const size_t imageViewCount = presentation.swapchainImageViews.size();
			for (size_t i = 0; i < imageViewCount; ++i) {
				vkDestroyImageView(r_device, presentation.swapchainImageViews[i], nullptr);
			}

			vkDestroySwapchainKHR(r_device, presentation.swapchain, nullptr);
		}

		if (execution.commandPool) {
			vkDestroyCommandPool(r_device, execution.commandPool, nullptr);
		}
		
		if (rendering.pipeline) {
			vkDestroyPipeline(r_device, rendering.pipeline, nullptr);
			vkDestroyPipelineLayout(r_device, rendering.pipelineLayout, nullptr);
		}
		
		if (bindings.descriptorPool) {
			vkDestroyDescriptorPool(r_device, bindings.descriptorPool, nullptr);
			vkDestroyDescriptorSetLayout(r_device, bindings.descriptorSetLayout, nullptr);
			vkDestroySampler(r_device, bindings.sampler, nullptr);
		}
		
		if (passes.compositionPass) {
			vkDestroyRenderPass(r_device, passes.compositionPass, nullptr);
		}
		
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