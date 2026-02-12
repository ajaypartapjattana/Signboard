#include "pipeline_builder.h"

#include "Signboard/RHI/primitive/pipeline.h"
#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/core/swapchain_vk.h"
#include "Signboard/RHI/primitive/pipelineLayout_vk.h"
#include "Signboard/RHI/primitive/shader_vk.h"


namespace rhi::procedure {

	pipeline_builder::pipeline_builder(const rhi::core::device& device, const rhi::core::swapchain& swapchain, const rhi::primitive::pipelineLayout& pipelineLayout)
		: m_device(rhi::core::device_vkAccess::get(device)), m_layout(rhi::primitive::pipelineLayout_vkAccess::get(pipelineLayout)), m_viewportExtent(rhi::core::swapchain_vkAccess::get_extent(swapchain))
	{
	
	}

	pipeline_builder& pipeline_builder::set_vertShader(const rhi::primitive::shader& s) noexcept {
		m_modules.push_back(rhi::primitive::shader_vkAccess::get(s));
		m_moduleRef.push_back(VK_SHADER_STAGE_VERTEX_BIT);

		return *this;
	}

	pipeline_builder& pipeline_builder::set_fragShader(const rhi::primitive::shader& s) noexcept {
		m_modules.push_back(rhi::primitive::shader_vkAccess::get(s));
		m_moduleRef.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);

		return *this;
	}

	VkResult pipeline_builder::build_graphics(rhi::primitive::pipeline& tw_pipeline) {

		if (m_moduleRef.empty())
			return VK_INCOMPLETE;
		std::vector<VkPipelineShaderStageCreateInfo> shaders;
		shaders.reserve(m_moduleRef.size());
		VkPipelineShaderStageCreateInfo shaderStageInfo{};
		shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfo.pName = "main";
		for (uint32_t i = 0; i < m_moduleRef.size(); ++i) {
			shaderStageInfo.stage = m_moduleRef[i];
			shaderStageInfo.module = m_modules[i];
			shaders.push_back(shaderStageInfo);
		}

		std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicStateInfo.pDynamicStates = dynamicStates.data();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		viewport.width = m_viewportExtent.width;
		viewport.height = m_viewportExtent.height;

		VkRect2D scissor{};
		scissor.offset = { 0,0 };
		scissor.extent = m_viewportExtent;

		VkPipelineViewportStateCreateInfo viewportStateInfo{};
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizerStateInfo{};
		rasterizerStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerStateInfo.depthClampEnable = VK_FALSE;
		rasterizerStateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerStateInfo.lineWidth = 1.0f;
		rasterizerStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizerStateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerStateInfo.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampleStateInfo{};
		multisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateInfo.sampleShadingEnable = VK_FALSE;
		multisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorblendAttachment{};
		colorblendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorblendAttachment.blendEnable = VK_TRUE;
		colorblendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorblendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorblendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorblendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorblendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorblendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorblendStateInfo{};
		colorblendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorblendStateInfo.attachmentCount = 1;
		colorblendStateInfo.pAttachments = &colorblendAttachment;

		VkGraphicsPipelineCreateInfo pipeInfo{};
		pipeInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeInfo.pStages = shaders.data();
		pipeInfo.pDynamicState = &dynamicStateInfo;
		pipeInfo.pVertexInputState = &vertexInputInfo;
		pipeInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipeInfo.pViewportState = &viewportStateInfo;
		pipeInfo.pRasterizationState = &rasterizerStateInfo;
		pipeInfo.pMultisampleState = &multisampleStateInfo;
		pipeInfo.layout = m_layout;

		VkPipeline vk_pipeline = VK_NULL_HANDLE;
		VkResult result =  vkCreateGraphicsPipelines(m_device, nullptr, 1, &pipeInfo, nullptr, &vk_pipeline);

		tw_pipeline.m_pipeline = vk_pipeline;
		tw_pipeline.m_device = m_device;

		return result;
	}

}