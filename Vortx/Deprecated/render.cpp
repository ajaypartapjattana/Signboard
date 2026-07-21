#include "render.h"

#include "Signboard/Renderer/Vertex/vertex.hh"
#include "Signboard/Assets/io/ResourceLoaders/file_loader.h"

#include <iostream>

namespace rndr {

	RenderStage::RenderStage(GLFWwindow* window) {
		createInstance();

		createDevice(window, 1);
		
		descriptorSetLayouts.root(device);
		pipelineLayouts.root(device);

		selectPresentationConfiguration();
		createSwapchain();

		createDescriptorSetLayouts();
		createPipelineLayouts();
	}

	void RenderStage::createDevice(GLFWwindow* window, uint32_t physicalDeviceIndex) {
		rhi::surface l_surface{ instance, window };
		const rhi::physicalDevice& r_selectedDevice = physicalDevices[physicalDeviceIndex];

		std::array<uint32_t, FAMILY_INDEX_MAX_ENUM> l_assignedFamilies{};
		l_assignedFamilies.fill(UINT32_MAX);

		uint32_t l_assignedPresentFamily = UINT32_MAX;

		uint32_t deviceFamilyCount;
		{
			deviceFamilyCount = static_cast<uint32_t>(r_selectedDevice.queueFamilies.size());

			std::array<uint32_t, FAMILY_INDEX_MAX_ENUM> familyCapabilities{};
			familyCapabilities.fill(UINT32_MAX);

			constexpr VkQueueFlags relevantFamilyCapabilityMask = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
			constexpr std::array<VkQueueFlags, FAMILY_INDEX_MAX_ENUM> familyFlags{ VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT };

			for (uint32_t i = 0; i < deviceFamilyCount; ++i) {
				const VkQueueFamilyProperties& familyProperty = r_selectedDevice.queueFamilies[i];

				uint32_t queueFamilyCapabilityCount = __popcnt(familyProperty.queueFlags & relevantFamilyCapabilityMask);

				for (uint32_t j = 0; j < FAMILY_INDEX_MAX_ENUM; ++j) {
					if ((familyProperty.queueFlags & familyFlags[j]) && queueFamilyCapabilityCount < familyCapabilities[j]) {
						l_assignedFamilies[j] = i;
						familyCapabilities[j] = queueFamilyCapabilityCount;
					}
				}
			}

			for (uint32_t i = 0; i < FAMILY_INDEX_MAX_ENUM; ++i) {
				if (l_assignedFamilies[i] == UINT32_MAX)
					throw std::runtime_error("INCAPABILITY: physical_device_has_insufficient_traits!");
			}

			sgb::span<const VkBool32> familyPresentationSupport = r_selectedDevice.queueFamilyPresentationSupport(l_surface);

			uint32_t graphicsFamilyIndex = l_assignedFamilies[FAMILY_INDEX_GRAPHICS];
			if (familyPresentationSupport[graphicsFamilyIndex]) {
				l_assignedPresentFamily = graphicsFamilyIndex;
			}
			else {
				for (uint32_t i = 0; i < deviceFamilyCount; ++i) {
					if (familyPresentationSupport[i]) {
						l_assignedPresentFamily = i;
						break;
					}
				}
			}

			if (l_assignedPresentFamily == UINT32_MAX)
				throw std::runtime_error("INCAPABILITY: physical_device_does_not_support_presentation!");
		}

		{
			float queueSchedulingPriorities = 1.0f;
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

			{
				std::vector<uint8_t> requiredFamilies(deviceFamilyCount, false);

				for (uint32_t i = 0; i < FAMILY_INDEX_MAX_ENUM; ++i) {
					uint32_t familyIndex = l_assignedFamilies[i];
					requiredFamilies[familyIndex] = true;
				}

				requiredFamilies[l_assignedPresentFamily] = true;

				VkDeviceQueueCreateInfo queueInfo{};
				queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueInfo.pNext = nullptr;
				queueInfo.flags = 0;
				queueInfo.queueCount = 1;
				queueInfo.pQueuePriorities = &queueSchedulingPriorities;

				queueCreateInfos.reserve(deviceFamilyCount);

				for (uint32_t i = 0; i < deviceFamilyCount; ++i) {
					if (requiredFamilies[i] == false)
						continue;

					queueInfo.queueFamilyIndex = i;

					queueCreateInfos.emplace_back(queueInfo);
				}

			}

			constexpr std::array deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
			VkPhysicalDeviceFeatures enabledFeatures{};
			enabledFeatures.samplerAnisotropy = VK_TRUE;

			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			createInfo.ppEnabledExtensionNames = deviceExtensions.data();
			createInfo.pEnabledFeatures = &enabledFeatures;

			VkResult result = device.create(physicalDevices[physicalDeviceIndex], &createInfo);

			if (result != VK_SUCCESS)
				throw std::runtime_error("FAILURE: device_creation");

			activePhysicalDeviceIndex = physicalDeviceIndex;
			surface = std::move(l_surface);
		}

		VmaAllocatorCreateInfo createInfo{};
		createInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
		createInfo.physicalDevice = r_selectedDevice;
		createInfo.device = device;
		createInfo.instance = instance;
		createInfo.vulkanApiVersion = VK_API_VERSION_1_3;

		VkResult result = allocator.create(&createInfo);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: allocator_creation!");

		assignedQueueFamilies = std::move(l_assignedFamilies);
		assignedPresentFamily = l_assignedPresentFamily;
	}

	void RenderStage::createSwapchain() {
		
	}

	void RenderStage::createDescriptorSetLayouts() {
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = 0;
		descriptorSetLayoutCreateInfo.pBindings = nullptr;

		std::vector<VkDescriptorSetLayoutBinding> bindings;

		{
			bindings.clear();
			bindings.reserve(2);

			{
				VkDescriptorSetLayoutBinding cameraUBO{};
				cameraUBO.binding = 0;
				cameraUBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				cameraUBO.descriptorCount = 1;
				cameraUBO.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
				cameraUBO.pImmutableSamplers = nullptr;

				bindings.push_back(cameraUBO);
			}

			{
				VkDescriptorSetLayoutBinding lightingSSBO{};
				lightingSSBO.binding = 1;
				lightingSSBO.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				lightingSSBO.descriptorCount = 1;
				lightingSSBO.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				lightingSSBO.pImmutableSamplers = nullptr;

				bindings.push_back(lightingSSBO);
			}

			descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			descriptorSetLayoutCreateInfo.pBindings = bindings.data();

			VkResult result = descriptorSetLayouts.create(&descriptorSetLayoutCreateInfo, &uniqueDescriptorSetLayoutIndices.gloabalSetLayout);
			if (result != VK_SUCCESS)
				throw std::runtime_error("FAILURE: descriptor_set_layout_creation!");
		}

		{
			bindings.clear();
			bindings.reserve(4);

			{
				VkDescriptorSetLayoutBinding materialUBO{};
				materialUBO.binding = 0;
				materialUBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				materialUBO.descriptorCount = 1;
				materialUBO.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				materialUBO.pImmutableSamplers = nullptr;

				bindings.push_back(materialUBO);
			}

			{
				VkDescriptorSetLayoutBinding albedo{};
				albedo.binding = 1;
				albedo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				albedo.descriptorCount = 1;
				albedo.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				albedo.pImmutableSamplers = nullptr;

				bindings.push_back(albedo);
			}

			{
				VkDescriptorSetLayoutBinding normal{};
				normal.binding = 2;
				normal.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				normal.descriptorCount = 1;
				normal.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				normal.pImmutableSamplers = nullptr;

				bindings.push_back(normal);
			}

			{
				VkDescriptorSetLayoutBinding roughness{};
				roughness.binding = 3;
				roughness.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				roughness.descriptorCount = 1;
				roughness.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				roughness.pImmutableSamplers = nullptr;

				bindings.push_back(roughness);
			}

			descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			descriptorSetLayoutCreateInfo.pBindings = bindings.data();

			VkResult result = descriptorSetLayouts.create(&descriptorSetLayoutCreateInfo, &uniqueDescriptorSetLayoutIndices.materialSetLayout);
			if (result != VK_SUCCESS)
				throw std::runtime_error("FAILURE: descriptor_set_layout_creation!");
		}

		bindings.clear();
		bindings.reserve(1);

		{
			VkDescriptorSetLayoutBinding objectUBO{};
			objectUBO.binding = 0;
			objectUBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			objectUBO.descriptorCount = 1;
			objectUBO.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			objectUBO.pImmutableSamplers = nullptr;

			bindings.push_back(objectUBO);
		}

		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		descriptorSetLayoutCreateInfo.pBindings = bindings.data();

		VkResult result = descriptorSetLayouts.create(&descriptorSetLayoutCreateInfo, &uniqueDescriptorSetLayoutIndices.objectSetLayout);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: descriptor_set_layout_creation!");
	}

	void RenderStage::createPipelineLayouts() {
		VkPipelineLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.setLayoutCount = 0;
		createInfo.pSetLayouts = nullptr;
		createInfo.pushConstantRangeCount = 0;
		createInfo.pPushConstantRanges = nullptr;

		std::vector<VkDescriptorSetLayout> l_descriptorSetLayouts;

		{
			l_descriptorSetLayouts.clear();
			l_descriptorSetLayouts.reserve(2);

			l_descriptorSetLayouts.push_back(descriptorSetLayouts[uniqueDescriptorSetLayoutIndices.materialSetLayout]);
			l_descriptorSetLayouts.push_back(descriptorSetLayouts[uniqueDescriptorSetLayoutIndices.objectSetLayout]);

			createInfo.setLayoutCount = static_cast<uint32_t>(l_descriptorSetLayouts.size());
			createInfo.pSetLayouts = l_descriptorSetLayouts.data();

			VkResult result = pipelineLayouts.create(&createInfo, &uniquePipelineLayoutIndices.PipelineLayout_2D);
			if (result != VK_SUCCESS)
				throw std::runtime_error("FAILURE: pipeline_layout_creation!");
		}

		l_descriptorSetLayouts.clear();
		l_descriptorSetLayouts.reserve(3);

		l_descriptorSetLayouts.push_back(descriptorSetLayouts[uniqueDescriptorSetLayoutIndices.gloabalSetLayout]);
		l_descriptorSetLayouts.push_back(descriptorSetLayouts[uniqueDescriptorSetLayoutIndices.materialSetLayout]);
		l_descriptorSetLayouts.push_back(descriptorSetLayouts[uniqueDescriptorSetLayoutIndices.objectSetLayout]);

		createInfo.setLayoutCount = static_cast<uint32_t>(l_descriptorSetLayouts.size());
		createInfo.pSetLayouts = l_descriptorSetLayouts.data();

		VkResult result = pipelineLayouts.create(&createInfo, &uniquePipelineLayoutIndices.PipelineLayout_3D);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: pipeline_layout_creation!");
	}

	void RenderStage::createRenderMethods(VkFormat backbufferImageFormat) {
		contextual_pool<rhi::renderPass> l_renderPasses{ device };

		{
			std::vector<VkAttachmentDescription> attachments;
			attachments.reserve(2);

			{
				VkAttachmentDescription colorOutput{};
				colorOutput.flags = 0;
				colorOutput.format = backbufferImageFormat;
				colorOutput.samples = VK_SAMPLE_COUNT_1_BIT;
				colorOutput.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				colorOutput.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				colorOutput.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				colorOutput.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				colorOutput.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				colorOutput.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				attachments.push_back(colorOutput);

				VkAttachmentDescription depthOutput{};
				depthOutput.flags = 0;
				depthOutput.format = VK_FORMAT_D32_SFLOAT;
				depthOutput.samples = VK_SAMPLE_COUNT_1_BIT;
				depthOutput.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				depthOutput.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthOutput.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				depthOutput.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthOutput.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				depthOutput.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				attachments.push_back(depthOutput);
			}

			std::vector<VkAttachmentReference> colorReferences;

			{
				VkAttachmentReference reference{};
				reference.attachment = 0;
				reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				colorReferences.push_back(reference);
			}

			VkAttachmentReference depthReference{};
			depthReference.attachment = 1;
			depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			std::vector<VkSubpassDescription> subpasses;

			{
				VkSubpassDescription subpass{};
				subpass.flags = 0;
				subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.inputAttachmentCount = 0;
				subpass.pInputAttachments = nullptr;
				subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
				subpass.pColorAttachments = colorReferences.data();
				subpass.pResolveAttachments = nullptr;
				subpass.pDepthStencilAttachment = &depthReference;
				subpass.preserveAttachmentCount = 0;
				subpass.pPreserveAttachments = nullptr;

				subpasses.push_back(subpass);
			}

			std::vector<VkSubpassDependency> dependancies;

			{
				VkSubpassDependency dependancy{};
				dependancy.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependancy.dstSubpass = 0;
				dependancy.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependancy.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependancy.srcAccessMask = 0;
				dependancy.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependancy.dependencyFlags = 0;

				dependancies.push_back(dependancy);
			}

			VkRenderPassCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			createInfo.pAttachments = attachments.data();
			createInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
			createInfo.pSubpasses = subpasses.data();
			createInfo.dependencyCount = static_cast<uint32_t>(dependancies.size());
			createInfo.pDependencies = dependancies.data();

			VkResult result = l_renderPasses.create(&createInfo, &uniqueRenderPassIndices.backbufferRenderPass);
			if (result != VK_SUCCESS)
				throw std::runtime_error("FAILURE: renderPass_creation!");
		}

		// --- --- --- PIPELINE CREATION --- --- ---

		contextual_pool<rhi::graphicsPipeline> l_graphicsPipelines{ device };

		{
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

			VkPipelineColorBlendStateCreateInfo colorBlendState{};
			colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlendState.pNext = nullptr;
			colorBlendState.flags = 0;
			colorBlendState.logicOpEnable = VK_FALSE;
			colorBlendState.logicOp = VK_LOGIC_OP_COPY;
			colorBlendState.attachmentCount = 0;
			colorBlendState.pAttachments = nullptr;
			colorBlendState.blendConstants[0] = 0.0f;
			colorBlendState.blendConstants[1] = 0.0f;
			colorBlendState.blendConstants[2] = 0.0f;
			colorBlendState.blendConstants[3] = 0.0f;

			VkPipelineDynamicStateCreateInfo dynamicState{};
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.pNext = nullptr;
			dynamicState.flags = 0;
			dynamicState.dynamicStateCount = 0;
			dynamicState.pDynamicStates = nullptr;

			VkGraphicsPipelineCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.stageCount = 0;
			createInfo.pStages = nullptr;
			createInfo.pVertexInputState = &vertexInputState;
			createInfo.pInputAssemblyState = &inputAssemblyState;
			createInfo.pTessellationState = nullptr;
			createInfo.pViewportState = &viewportState;
			createInfo.pRasterizationState = &rasterizationState;
			createInfo.pMultisampleState = &multisampleState;
			createInfo.pDepthStencilState = &depthStencilState;
			createInfo.pColorBlendState = &colorBlendState;
			createInfo.pDynamicState = &dynamicState;
			createInfo.layout = VK_NULL_HANDLE;
			createInfo.renderPass = VK_NULL_HANDLE;
			createInfo.subpass = 0;
			createInfo.basePipelineHandle = VK_NULL_HANDLE;
			createInfo.basePipelineIndex = 0;

			{
				contextual_pool<rhi::shaderModule> shaderModules{ device };

				std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

				{
					io::loader::file_loader shader_loader;

					VkShaderModuleCreateInfo createInfo{};
					createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
					createInfo.pNext = nullptr;
					createInfo.flags = 0;
					createInfo.codeSize = 0;
					createInfo.pCode = nullptr;

					VkPipelineShaderStageCreateInfo stageInfo{};
					stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
					stageInfo.pNext = nullptr;
					stageInfo.flags = 0;
					stageInfo.pName = "main";
					stageInfo.module = VK_NULL_HANDLE;
					stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

					struct shaderAttachment {
						VkShaderStageFlagBits stage;
						const std::filesystem::path path;
					};

					std::array<shaderAttachment, 2> shaderList{ {
						{VK_SHADER_STAGE_VERTEX_BIT, "shaders/base.vert"},
						{VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/base.frag"}
					} };

					size_t shaderCount = shaderList.size();

					shaderStages.reserve(shaderCount);

					for (size_t i = 0; i < shaderCount; ++i) {
						auto shaderData = shader_loader.load_SPIRV(shaderList[i].path);

						createInfo.codeSize = shaderData.size() * sizeof(uint32_t);
						createInfo.pCode = shaderData.data();

						size_t index;
						VkResult result = shaderModules.create(&createInfo, &index);
						if (result != VK_SUCCESS)
							throw std::runtime_error("FAILURE: shader_creation!");

						stageInfo.module = shaderModules[index];
						stageInfo.stage = shaderList[i].stage;

						shaderStages.push_back(stageInfo);
					}
				}

				createInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
				createInfo.pStages = shaderStages.data();

				auto vertexAttributes = Vertex_3D::attributes();
				VkVertexInputBindingDescription vertexBindings = Vertex_3D::binding();

				vertexInputState.vertexBindingDescriptionCount = 1;
				vertexInputState.pVertexBindingDescriptions = &vertexBindings;
				vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
				vertexInputState.pVertexAttributeDescriptions = vertexAttributes.data();

				std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;

				{
					VkPipelineColorBlendAttachmentState attachment{};
					attachment.blendEnable = VK_TRUE;
					attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
					attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
					attachment.colorBlendOp = VK_BLEND_OP_ADD;
					attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
					attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
					attachment.alphaBlendOp = VK_BLEND_OP_ADD;
					attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

					colorBlendAttachments.push_back(attachment);
				}

				colorBlendState.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
				colorBlendState.pAttachments = colorBlendAttachments.data();

				std::vector<VkDynamicState> dynamicStates{
					VK_DYNAMIC_STATE_VIEWPORT,
					VK_DYNAMIC_STATE_SCISSOR
				};

				dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
				dynamicState.pDynamicStates = dynamicStates.data();

				createInfo.layout = pipelineLayouts[uniquePipelineLayoutIndices.PipelineLayout_3D];
				createInfo.renderPass = l_renderPasses[uniqueRenderPassIndices.backbufferRenderPass];

				VkResult result = l_graphicsPipelines.create(&createInfo, &uniquePipelineIndices.pipeline_3D);
				if (result != VK_SUCCESS)
					throw std::runtime_error("FAILURE: graphics_pipeline_creation!");

			}
		}

		renderPasses = std::move(l_renderPasses);
		grpahicsPipelines = std::move(l_graphicsPipelines);
	}

	VkFramebuffer RenderStage::createFramebuffer(VkRenderPass renderPass, sgb::span<const VkImageView> attachments, VkExtent2D extent) {
		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.renderPass = renderPass;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = extent.width;
		createInfo.height = extent.height;
		createInfo.layers = 1;

		size_t index;
		VkResult result = framebuffers.create(&createInfo, &index);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: framebuffer_creation!");

		return framebuffers[index];
	}

}
