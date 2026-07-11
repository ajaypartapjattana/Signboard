#include "presentation.h"

#include "core/Math/search.h"
#include "assets/io/ResourceLoaders/file_loader.h"

#include <array>
#include <vector>

namespace rndr {

	struct SurfaceFormatEq {
		constexpr bool operator()(const VkSurfaceFormatKHR& a, const VkSurfaceFormatKHR& b) const noexcept {
			return a.format == b.format && a.colorSpace == b.colorSpace;
		}
	};

	VkResult presentationStage::root(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const PresentationStageQueueInfo* pQueueInfo) noexcept {
		VkResult result;
		
		if (!presentationTargets.empty()) {
			return VK_ERROR_NOT_PERMITTED;
		}

		VkSurfaceFormatKHR surfaceFormat;

		{
			uint32_t supportedFormatCount = 0;
			result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &supportedFormatCount, nullptr);
			if (result != VK_SUCCESS)
				return result;

			std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &supportedFormatCount, supportedFormats.data());

			constexpr std::array preferredSurfaceFormats {
				VkSurfaceFormatKHR{ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
				VkSurfaceFormatKHR{ VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
				VkSurfaceFormatKHR{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
				VkSurfaceFormatKHR{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }
			};

			size_t index = 0;
			std::findEarly_basic<VkSurfaceFormatKHR, SurfaceFormatEq>(preferredSurfaceFormats.data(), preferredSurfaceFormats.size(), supportedFormats.data(), supportedFormats.size(), &index);

			surfaceFormat = supportedFormats[index];
		}
		
		struct {
			VkRenderPass renderPass;
			VkSampler sampler;
			VkDescriptorSetLayout descriptorSetLayout;
			VkPipelineLayout pipelineLayout;
			VkPipeline pipeline;
			VkCommandPool commandPool;
		} local;

		memset(&local, 0, sizeof(local));

		PresentStageJob job;

		memset(&job, 0, sizeof(job));

		do {

			{
				VkAttachmentDescription output{};
				output.format = surfaceFormat.format;
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

				result = vkCreateRenderPass(device, &createInfo, nullptr, &local.renderPass);
			}

			if (result != VK_SUCCESS)
				break;

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

				result = vkCreateSampler(device, &createInfo, nullptr, &local.sampler);
			}

			if (result != VK_SUCCESS)
				break;

			{
				VkDescriptorSetLayoutBinding image{};
				image.binding = 0;
				image.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				image.descriptorCount = 1;
				image.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				image.pImmutableSamplers = &local.sampler;

				VkDescriptorSetLayoutCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				createInfo.bindingCount = 1;
				createInfo.pBindings = &image;

				result = vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &local.descriptorSetLayout);
			}

			if (result != VK_SUCCESS)
				break;

			{
				VkPipelineLayoutCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				createInfo.setLayoutCount = 1;
				createInfo.pSetLayouts = &local.descriptorSetLayout;
				createInfo.pushConstantRangeCount = 0;
				createInfo.pPushConstantRanges = nullptr;

				result = vkCreatePipelineLayout(device, &createInfo, nullptr, &local.pipelineLayout);
			}

			if (result != VK_SUCCESS)
				break;

			{
				VkShaderModule vertexModule;

				{
					std::vector<uint32_t> spirv = io::loader::file_loader::load_SPIRV("shaders/presentation.vert.spv");

					VkShaderModuleCreateInfo createInfo{};
					createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
					createInfo.pNext = nullptr;
					createInfo.flags = 0;
					createInfo.codeSize = spirv.size() * sizeof(uint32_t);
					createInfo.pCode = spirv.data();

					result = vkCreateShaderModule(device, &createInfo, nullptr, &vertexModule);
				}

				if (result != VK_SUCCESS)
					break;

				VkShaderModule fragmentModule;

				{
					std::vector<uint32_t> spirv = io::loader::file_loader::load_SPIRV("shaders/presentation.frag.spv");

					VkShaderModuleCreateInfo createInfo{};
					createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
					createInfo.pNext = nullptr;
					createInfo.flags = 0;
					createInfo.codeSize = spirv.size() * sizeof(uint32_t);
					createInfo.pCode = spirv.data();

					result = vkCreateShaderModule(device, &createInfo, nullptr, &fragmentModule);
				}

				if (result != VK_SUCCESS) {
					vkDestroyShaderModule(device, vertexModule, nullptr);

					break;
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
				createInfo.layout = local.pipelineLayout;
				createInfo.renderPass = local.renderPass;
				createInfo.subpass = 0;
				createInfo.basePipelineHandle = VK_NULL_HANDLE;
				createInfo.basePipelineIndex = 0;

				result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &local.pipeline);

				vkDestroyShaderModule(device, fragmentModule, nullptr);
				vkDestroyShaderModule(device, vertexModule, nullptr);
			}

			if (result != VK_SUCCESS)
				break;

			{
				VkCommandPoolCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
				createInfo.queueFamilyIndex = pQueueInfo->graphicsFamilyIndex;

				result = vkCreateCommandPool(device, &createInfo, nullptr, &local.commandPool);
			}

			if (result != VK_SUCCESS)
				break;

			{
				VkFenceCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

				result = vkCreateFence(device, &createInfo, nullptr, &job.inFlight);
			}

			if (result != VK_SUCCESS)
				break;

			{
				VkSemaphoreCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;

				result = vkCreateSemaphore(device, &createInfo, nullptr, &job.imageAvailable);
			}

			if (result != VK_SUCCESS)
				break;

			{
				VkSemaphoreCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;

				result = vkCreateSemaphore(device, &createInfo, nullptr, &job.renderComplete);
			}

			if (result != VK_SUCCESS)
				break;

			{
				VkCommandBufferAllocateInfo allocateInfo{};
				allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocateInfo.pNext = nullptr;
				allocateInfo.commandPool = local.commandPool;
				allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocateInfo.commandBufferCount = 1;

				result = vkAllocateCommandBuffers(device, &allocateInfo, &job.commandBuffer);
			}

			if (result != VK_SUCCESS)
				break;

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

			jobs.reserve(maxConcurrentPresentations);
			jobs.emplace_back(job);

			acquireHint = 0;

			if (execution.commandPool)
				vkDestroyCommandPool(r_device, execution.commandPool, nullptr);

			execution.commandPool = local.commandPool;

			execution.graphicsFamily = pQueueInfo->graphicsFamilyIndex;
			execution.graphicsQueue = pQueueInfo->graphicsQueue;
			execution.presentFamily = pQueueInfo->presentFamilyIndex;
			execution.presentQueue = pQueueInfo->presentQueue;

			if (rendering.pipeline)
				vkDestroyPipeline(r_device, rendering.pipeline, nullptr);

			rendering.pipeline = local.pipeline;

			if (rendering.pipelineLayout)
				vkDestroyPipelineLayout(r_device, rendering.pipelineLayout, nullptr);

			rendering.pipelineLayout = local.pipelineLayout;

			if (bindings.descriptorSetLayout)
				vkDestroyDescriptorSetLayout(r_device, bindings.descriptorSetLayout, nullptr);

			bindings.descriptorSetLayout = local.descriptorSetLayout;

			if (bindings.sampler)
				vkDestroySampler(r_device, bindings.sampler, nullptr);

			bindings.sampler = local.sampler;

			if (passes.compositionPass)
				vkDestroyRenderPass(r_device, passes.compositionPass, nullptr);

			passes.compositionPass = local.renderPass;

			configuration.surfaceFormat = surfaceFormat;

			r_device = device;
			r_physicalDevice = physicalDevice;

			return VK_SUCCESS;

		} while (false);

		if (job.renderComplete)
			vkDestroySemaphore(device, job.renderComplete, nullptr);

		if(job.imageAvailable)
			vkDestroySemaphore(device, job.imageAvailable, nullptr);

		if (job.inFlight)
			vkDestroyFence(device, job.inFlight, nullptr);

		if (local.commandPool)
			vkDestroyCommandPool(device, local.commandPool, nullptr);

		if (local.pipeline)
			vkDestroyPipeline(device, local.pipeline, nullptr);

		if (local.pipelineLayout)
			vkDestroyPipelineLayout(device, local.pipelineLayout, nullptr);

		if (local.descriptorSetLayout)
			vkDestroyDescriptorSetLayout(device, local.descriptorSetLayout, nullptr);

		if (local.sampler)
			vkDestroySampler(device, local.sampler, nullptr);

		if (local.renderPass)
			vkDestroyRenderPass(device, local.renderPass, nullptr);

		return result;
	}

	VkResult presentationStage::targetSurface(VkSurfaceKHR surface, size_t* pIndex) noexcept {
		VkResult result;

		if (!pIndex)
			return VK_ERROR_INVALID_EXTERNAL_HANDLE;

		{
			VkBool32 surfaceSupport;
			result = vkGetPhysicalDeviceSurfaceSupportKHR(r_physicalDevice, execution.presentFamily, surface, &surfaceSupport);
			if (result != VK_SUCCESS)
				return result;
				
			if (!surfaceSupport)
				return VK_ERROR_NOT_PERMITTED;
		}

		{
			uint32_t supportedFormatCount = 0;
			result = vkGetPhysicalDeviceSurfaceFormatsKHR(r_physicalDevice, surface, &supportedFormatCount, nullptr);
			if (result != VK_SUCCESS)
				return result;

			std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(r_physicalDevice, surface, &supportedFormatCount, supportedFormats.data());

			bool support = std::contain<VkSurfaceFormatKHR, SurfaceFormatEq>(configuration.surfaceFormat, supportedFormats.data(), supportedFormats.size());
			if (!support)
				return VK_ERROR_FORMAT_NOT_SUPPORTED;
		}
		
		VkPresentModeKHR surfacePresentMode = VK_PRESENT_MODE_FIFO_KHR;

		{
			uint32_t supportedPresentModeCount = 0;
			result = vkGetPhysicalDeviceSurfacePresentModesKHR(r_physicalDevice, surface, &supportedPresentModeCount, nullptr);
			if (result != VK_SUCCESS)
				return result;

			std::vector<VkPresentModeKHR> supportedPresentModes(supportedPresentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(r_physicalDevice, surface, &supportedPresentModeCount, supportedPresentModes.data());

			if (std::contain(VK_PRESENT_MODE_MAILBOX_KHR, supportedPresentModes.data(), supportedPresentModes.size())) {
				surfacePresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			}
		}

		*pIndex = presentationTargets.size();

		PresentationTarget& target = presentationTargets.emplace_back();

		target.surface = surface;
		target.presentMode = surfacePresentMode;

		return VK_SUCCESS;
	}

	void presentationStage::releaseSurface(size_t index, VkSurfaceKHR* pSurface) noexcept {
		if (!pSurface)
			return;

		const PresentationTarget& target = presentationTargets[index];

		const size_t imageCount = target.framebuffers.size();

		for (uint32_t i = 0; i < imageCount; ++i) {
			VkFence fence = target.imageFence[i];
			if (!fence)
				continue;

			VkResult result = vkWaitForFences(r_device, 1, &fence, VK_TRUE, UINT64_MAX);
			if (result != VK_SUCCESS) {
				*pSurface = VK_NULL_HANDLE;

				return;
			}
		}

		for (size_t i = 0; i < imageCount; ++i) {
			vkDestroyFramebuffer(r_device, target.framebuffers[i], nullptr);
			vkDestroyImageView(r_device, target.imageViews[i], nullptr);
		}

		if (target.swapchain)
			vkDestroySwapchainKHR(r_device, target.swapchain, nullptr);

		*pSurface = target.surface;
	}

	VkResult presentationStage::configurePresentation(size_t targetIndex, uint32_t minImageCount) noexcept {
		VkResult result;

		VkExtent2D extent;

		VkSwapchainKHR swapchain = VK_NULL_HANDLE;

		std::vector<VkImageView> imageViews;

		std::vector<VkFramebuffer> framebuffers;

		PresentationTarget& target = presentationTargets[targetIndex];

		do {

			VkSurfaceCapabilitiesKHR surfaceCapabilities{};
			
			result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(r_physicalDevice, target.surface, &surfaceCapabilities);
			
			if (result != VK_SUCCESS)
				break;
			
			extent = surfaceCapabilities.currentExtent;
			
			{
				VkSwapchainCreateInfoKHR createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				createInfo.surface = target.surface;
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
				createInfo.presentMode = target.presentMode;
				createInfo.clipped = VK_TRUE;
				createInfo.oldSwapchain = target.swapchain;

				result = vkCreateSwapchainKHR(r_device, &createInfo, nullptr, &swapchain);
			}

			if (result != VK_SUCCESS)
				break;

			uint32_t imageCount;

			result = vkGetSwapchainImagesKHR(r_device, swapchain, &imageCount, nullptr);

			if (result != VK_SUCCESS)
				break;

			std::vector<VkImage> swapchainImages(imageCount);
			
			result = vkGetSwapchainImagesKHR(r_device, swapchain, &imageCount, swapchainImages.data());

			if (result != VK_SUCCESS)
				break;

			imageViews.resize(imageCount, VK_NULL_HANDLE);
			framebuffers.resize(imageCount, VK_NULL_HANDLE);

			{
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

				for (size_t i = 0; i < imageCount; ++i) {
					createInfo.image = swapchainImages[i];
					
					result = vkCreateImageView(r_device, &createInfo, nullptr, &imageViews[i]);
					if (result != VK_SUCCESS)
						break;
				}
			}

			if (result != VK_SUCCESS)
				break;

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

			for (size_t i = 0; i < imageCount; ++i) {
				createInfo.pAttachments = &imageViews[i];

				result = vkCreateFramebuffer(r_device, &createInfo, nullptr, &framebuffers[i]);
				if (result != VK_SUCCESS)
					break;
			}

		} while (false);

		if (result == VK_SUCCESS) {
			const size_t oldImageCount = target.framebuffers.size();

			for (size_t i = 0; i < oldImageCount; ++i) {
				VkFence fence = target.imageFence[i];
				if (!fence)
					continue;

				result = vkWaitForFences(r_device, 1, &fence, VK_TRUE, UINT64_MAX);
				if (result != VK_SUCCESS)
					return result;
			}

			for (size_t i = 0; i < oldImageCount; ++i) {
				vkDestroyFramebuffer(r_device, target.framebuffers[i], nullptr);
				vkDestroyImageView(r_device, target.imageViews[i], nullptr);
			}
			
			target.imageFence.assign(framebuffers.size(), VK_NULL_HANDLE);

			target.framebuffers = std::move(framebuffers);
			target.imageViews = std::move(imageViews);

			target.extent = extent;

			if (target.swapchain)
				vkDestroySwapchainKHR(r_device, target.swapchain, nullptr);

			target.swapchain = swapchain;

			return VK_SUCCESS;
		}

		const size_t imageCount = framebuffers.size();

		for (size_t i = 0; i < imageCount; ++i) {
			if (framebuffers[i])
				vkDestroyFramebuffer(r_device, framebuffers[i], nullptr);

			if (imageViews[i])
				vkDestroyImageView(r_device, imageViews[i], nullptr);
		}

		if (swapchain)
			vkDestroySwapchainKHR(r_device, swapchain, nullptr);

		return result;
	}

	VkResult presentationStage::presentationImagePool(const VkImageView* pImageViews, uint32_t count) {
		VkResult result;
		
		VkDescriptorPool descriptorPool = nullptr;
		
		std::vector<VkDescriptorSet> descriptorSets(count, VK_NULL_HANDLE);

		do {

			{
				VkDescriptorPoolSize poolSize{};
				poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				poolSize.descriptorCount = count;

				VkDescriptorPoolCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				createInfo.maxSets = count;
				createInfo.poolSizeCount = 1;
				createInfo.pPoolSizes = &poolSize;

				result = vkCreateDescriptorPool(r_device, &createInfo, nullptr, &descriptorPool);
			}

			if (result != VK_SUCCESS)
				break;

			{
				VkDescriptorSetAllocateInfo allocateInfo{};
				allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocateInfo.pNext = nullptr;
				allocateInfo.descriptorPool = bindings.descriptorPool;
				allocateInfo.descriptorSetCount = count;
				allocateInfo.pSetLayouts = &bindings.descriptorSetLayout;

				result = vkAllocateDescriptorSets(r_device, &allocateInfo, descriptorSets.data());
			}

			if (result != VK_SUCCESS)
				break;

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

			if (bindings.descriptorPool)
				vkDestroyDescriptorPool(r_device, bindings.descriptorPool, nullptr);

			bindings.descriptorPool = descriptorPool;

			bindings.descriptorSets = std::move(descriptorSets);

			return VK_SUCCESS;

		} while (true);

		if (descriptorPool)
			vkDestroyDescriptorPool(r_device, descriptorPool, nullptr);
		
		return VK_SUCCESS;
	}

	VkResult presentationStage::stagePresentation(uint32_t imageIndex, uint32_t targetIndex, VkSemaphore waitSemaphore) {
		VkResult result;

		size_t jobIndex;

		{
			PresentStageJob& oldJob = jobs[acquireHint];
			
			result = vkGetFenceStatus(r_device, oldJob.inFlight);
			switch (result) {
			case VK_SUCCESS:
				jobIndex = acquireHint;
				acquireHint = (acquireHint + 1) % jobs.size();

				break;

			case VK_NOT_READY: {
				if (jobs.size() >= maxConcurrentPresentations) {
					result = vkWaitForFences(r_device, 1, &oldJob.inFlight, VK_TRUE, UINT32_MAX);
					if (result != VK_SUCCESS)
						return result;

					jobIndex = acquireHint;
					acquireHint = (acquireHint + 1) % jobs.size();

					break;
				}

				PresentStageJob job;

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

				{
					VkCommandBufferAllocateInfo allocateInfo{};
					allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
					allocateInfo.pNext = nullptr;
					allocateInfo.commandPool = execution.commandPool;
					allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
					allocateInfo.commandBufferCount = 1;

					result = vkAllocateCommandBuffers(r_device, &allocateInfo, &job.commandBuffer);
					if (result != VK_SUCCESS) {
						vkDestroySemaphore(r_device, job.renderComplete, nullptr);
						vkDestroySemaphore(r_device, job.imageAvailable, nullptr);
						vkDestroyFence(r_device, job.inFlight, nullptr);

						return result;
					}
				}

				jobIndex = jobs.size();
				jobs.emplace_back(job);

				break;
			}

			default:
				return result;
			}
			
		}
		
		PresentStageJob& job = jobs[jobIndex];

		PresentationTarget& target = presentationTargets[targetIndex];

		uint32_t availableImageIndex = 0;

		result = vkAcquireNextImageKHR(r_device, target.swapchain, UINT64_MAX, job.imageAvailable, VK_NULL_HANDLE, &availableImageIndex);
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
			renderArea.extent = target.extent;

			VkRenderPassBeginInfo pass{};
			pass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			pass.pNext = nullptr;
			pass.renderPass = passes.compositionPass;
			pass.framebuffer = target.framebuffers[availableImageIndex];
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
			viewport.width = (float)target.extent.width;
			viewport.height = (float)target.extent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

			VkRect2D scissors{};
			scissors.offset = { 0,0 };
			scissors.extent = target.extent;

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

		target.imageFence[availableImageIndex] = job.inFlight;

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &job.renderComplete;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &target.swapchain;
		presentInfo.pImageIndices = &availableImageIndex;
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(execution.presentQueue, &presentInfo);
		if (result != VK_SUCCESS)
			return result;

		return VK_SUCCESS;
	}
	
	void presentationStage::reset() noexcept {
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

		if (execution.commandPool) {
			vkDestroyCommandPool(r_device, execution.commandPool, nullptr);
		}

		if (rendering.pipeline) {
			vkDestroyPipeline(r_device, rendering.pipeline, nullptr);
			vkDestroyPipelineLayout(r_device, rendering.pipelineLayout, nullptr);
		}

		if (bindings.descriptorPool) {
			vkDestroyDescriptorPool(r_device, bindings.descriptorPool, nullptr);
		}

		if (bindings.descriptorSetLayout) {
			vkDestroyDescriptorSetLayout(r_device, bindings.descriptorSetLayout, nullptr);
			vkDestroySampler(r_device, bindings.sampler, nullptr);
		}

		if (passes.compositionPass) {
			vkDestroyRenderPass(r_device, passes.compositionPass, nullptr);
		}
	}

}
