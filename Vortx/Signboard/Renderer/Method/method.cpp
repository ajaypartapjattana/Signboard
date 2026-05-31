#include "method.hh"

#include "Signboard/Assets/shaderReflect/shaderReflect.h"
#include <map>

namespace rndr {

	uint32_t renderConfig::createBackbufferRenderContext(VkFormat swapchainImageFormat) {
		std::vector<VkAttachmentDescription> attachments;

		{
			VkAttachmentDescription attachment{};
			attachment.flags = 0;
			attachment.format = swapchainImageFormat;
			attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			attachments.push_back(attachment);
		}

		std::vector<VkAttachmentReference> colorReferences;

		{
			VkAttachmentReference reference{};
			reference.attachment = 0;
			reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			colorReferences.push_back(reference);
		}

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
			subpass.pDepthStencilAttachment = nullptr;
			subpass.preserveAttachmentCount = 0;
			subpass.pPreserveAttachments = nullptr;

			subpasses.push_back(subpass);
		}

		std::vector<VkSubpassDependency> dependancies;

		{
			VkSubpassDependency dependancy{};
			dependancy.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependancy.dstSubpass = 0;
			dependancy.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependancy.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependancy.srcAccessMask = 0;
			dependancy.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
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

		VkRenderPass renderPass = VK_NULL_HANDLE;
		VkResult result = renderPasses.create(&createInfo, &renderPass);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: renderPass_creation!");

		renderPassContext context{};
		context.renderPass = renderPass;

		uint32_t index = static_cast<uint32_t>(renderContexts.size());
		renderContexts.push_back(context);

		return index;
	}

	void renderConfig::createFramebuffer(uint32_t renderContextIndex, sgb::span<const VkImageView> attachments, VkExtent2D extent) {
		VkFramebufferCreateInfo createInfo{};

		renderPassContext& r_context = renderContexts[renderContextIndex];

		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.renderPass = r_context.renderPass;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = extent.width;
		createInfo.height = extent.height;
		createInfo.layers = 1;

		VkFramebuffer framebuffer = VK_NULL_HANDLE;
		VkResult result = framebuffers.create(&createInfo, &framebuffer);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: framebuffer_creation!");

		r_context.framebuffers.push_back(framebuffer);
		r_context.extent.push_back({ createInfo.width, createInfo.height });
	}

	void renderConfig::createRenderPipeline(uint32_t compatibilityContextIndex, renderPipelineShaderSet shaders) {
		VertexLayoutDesc vertexInputDescription;

		{
			ShaderReflect reflector;

			reflector.compile(shaders.vertexShaderBin.data);
			vertexInputDescription = reflector.reflectVertexLayout();
		}

		std::vector<VkViewport> viewports;
		std::vector<VkRect2D> scissors;

		{

		}

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

		std::vector<VkDynamicState> dynamicStates{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		
		VkPipelineLayout l_pipelineLayout = VK_NULL_HANDLE;

		{
			std::vector<VkDescriptorSetLayout> l_descriptorSetLayouts;

			{
				std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> grouped;

				{
					BindingMap merged;
					ShaderReflect reflector;

					auto shaderList = shaders.list();

					uint32_t shaderCount = static_cast<uint32_t>(shaderList.size());
					for (uint32_t i = 0; i < shaderCount; ++i) {
						reflector.compile(shaderList[i].data);

						auto local = reflector.reflectDescriptorBindings(shaderList[i].stage);

						for (auto& [key, binding] : local) {
							auto [it, inserted] = merged.try_emplace(key, std::move(binding));
							if (!inserted) {
								it->second.stageFlags |= binding.stageFlags;
							}
						}
					}

					for (const auto& [key, binding] : merged) {
						grouped[key.set].push_back(binding);
					}

				}

				VkDescriptorSetLayoutCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;

				l_descriptorSetLayouts.reserve(grouped.size());
				for (auto& [setIndex, bindings] : grouped) {
					createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
					createInfo.pBindings = bindings.data();

					VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
					VkResult result = descriptorSetLayouts.create(&createInfo, &descriptorSetLayout);
					if (result != VK_SUCCESS)
						throw std::runtime_error("FAILURE: descriptor_set_layout_creation!");

					l_descriptorSetLayouts.push_back(descriptorSetLayout);
				}
			}

			VkPipelineLayoutCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.setLayoutCount = static_cast<uint32_t>(l_descriptorSetLayouts.size());
			createInfo.pSetLayouts = l_descriptorSetLayouts.data();
			createInfo.pushConstantRangeCount = 0;
			createInfo.pPushConstantRanges = nullptr;

			VkResult result = pipelineLayouts.create(&createInfo, &l_pipelineLayout);
			if (result != VK_SUCCESS)
				throw std::runtime_error("FAILURE: pipeline_layout_creation!");
		}
		
		VkRenderPass l_renderPass = renderContexts[compatibilityContextIndex].renderPass;

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		{
			VkPipelineShaderStageCreateInfo stageInfo{};
			stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stageInfo.pNext = nullptr;
			stageInfo.flags = 0;
			stageInfo.pName = "main";

			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;

			auto shaderList = shaders.list();

			uint32_t shaderCount = static_cast<uint32_t>(shaderList.size());

			shaderStages.reserve(shaderCount);
			for (uint32_t i = 0; i < shaderCount; ++i) {
				createInfo.pCode = shaderList[i].data.data();
				createInfo.codeSize = shaderList[i].data.size() * sizeof(uint32_t);

				VkShaderModule shaderModule = VK_NULL_HANDLE;
				VkResult result = shaderModules.create(&createInfo, &shaderModule);
				if (result != VK_SUCCESS)
					throw std::runtime_error("FAILURE: shader_creation!");

				stageInfo.module = shaderModule;
				stageInfo.stage = shaderList[i].stage;

				shaderStages.push_back(stageInfo);
			}
		}


		VkPipelineVertexInputStateCreateInfo vertexInputState{};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.pNext = nullptr;
		vertexInputState.flags = 0;
		vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputDescription.bindings.size());
		vertexInputState.pVertexBindingDescriptions = vertexInputDescription.bindings.data();
		vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputDescription.attributes.size());
		vertexInputState.pVertexAttributeDescriptions = vertexInputDescription.attributes.data();

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
		depthStencilState.depthTestEnable= VK_TRUE;
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
		colorBlendState.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
		colorBlendState.pAttachments = colorBlendAttachments.data();
		colorBlendState.blendConstants[0] = 0.0f;
		colorBlendState.blendConstants[1] = 0.0f;
		colorBlendState.blendConstants[2] = 0.0f;
		colorBlendState.blendConstants[3] = 0.0f;

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.pNext = nullptr;
		dynamicState.flags = 0;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkGraphicsPipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		createInfo.pStages = shaderStages.data();
		createInfo.pVertexInputState = &vertexInputState;
		createInfo.pInputAssemblyState = &inputAssemblyState;
		createInfo.pTessellationState = &tessellationState;
		createInfo.pViewportState = &viewportState;
		createInfo.pRasterizationState = &rasterizationState;
		createInfo.pMultisampleState = &multisampleState;
		createInfo.pDepthStencilState = &depthStencilState;
		createInfo.pColorBlendState = &colorBlendState;
		createInfo.pDynamicState = &dynamicState;
		createInfo.layout = l_pipelineLayout;
		createInfo.renderPass = l_renderPass;
		createInfo.subpass = 0;
		createInfo.basePipelineHandle = VK_NULL_HANDLE;
		createInfo.basePipelineIndex = 0;

		VkPipeline pipeline = VK_NULL_HANDLE;
		VkResult result = grpahicsPipelines.create(&createInfo, &pipeline);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: graphics_pipeline_creation!");
	
		shaderModules.reset();
	}

	uint32_t renderConfig::loadShader(const char* path, VkShaderStageFlagBits stage) {
		io::loader::file_loader loader;

		sgb::vltWrite<shaderBinary> _wrt{ m_shaderBins };

		auto _ctor = [&](shaderBinary* bin) {
			bin->stage = stage;

			try {
				bin->data = loader.load_SPIRV(path);
			}
			catch (const std::exception& e) {
				LOG_ERROR("FAILURE: shader_load -> %s", path, e.what());
				throw;
			}
			};

		return _wrt.construct(_ctor);
	}

}
