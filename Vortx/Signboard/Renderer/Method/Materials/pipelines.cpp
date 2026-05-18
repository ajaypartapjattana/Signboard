#include "pipelines.h"

#include "Signboard/Assets/shaderReflect/shaderReflect.h"

namespace rndr {

	pipelines::pipelines(const rhi::creDevice& device, sgb::vltWrite<rhi::pmvPipeline> pipelineWrite)
		:
		_wrt(pipelineWrite),

		m_shaderCreate(device),
		m_pipelineCreate(device)
	{

	}

	uint32_t pipelines::createPipeline(const rhi::pmvRenderPass& renderPass, uint32_t subpass, const rhi::pmvPipelineLayout& pipelineLayout, sgb::span<const shaderBinary*> shaders) {
		if (shaders.empty())
			throw std::logic_error("FAILURE: invalid_input_size!");
		
		uint32_t _sSz = static_cast<uint32_t>(shaders.size());

		uint32_t vertexShaderIndex = UINT32_MAX;

		std::vector<rhi::pmvShader> _shaders(_sSz);
		for (uint32_t i = 0; i < _sSz; ++i) {
			m_shaderCreate.target_source(shaders[i]->data);
			VkResult result = m_shaderCreate.publish(_shaders[i], shaders[i]->stage);

			if (result != VK_SUCCESS)
				throw std::runtime_error("FAILURE: shader_module_creation!");

			if (vertexShaderIndex == UINT32_MAX && shaders[i]->stage == VK_SHADER_STAGE_VERTEX_BIT) {
				vertexShaderIndex = i;
			}
		}

		if (vertexShaderIndex == UINT32_MAX)
			throw std::runtime_error("INCOMPLETE: vertex_shader_missing!");

		m_pipelineCreate.bind_shaders(_shaders);
		
		{
			ShaderReflect vertexReflect{ shaders[vertexShaderIndex]->data };
			VertexLayoutDesc vertexLayout = vertexReflect.reflectVertexLayout();

			m_pipelineCreate.vertexInput(vertexLayout.bindings, vertexLayout.attributes);
		}

		std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		m_pipelineCreate.dynamicStates(dynamicStates);

		VkPipelineColorBlendAttachmentState colorblendAttachment{};
		colorblendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorblendAttachment.blendEnable = VK_TRUE;
		colorblendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorblendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorblendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorblendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorblendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorblendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		m_pipelineCreate.push_blendAttachments({ &colorblendAttachment, 1 });

		m_pipelineCreate.target_renderPass(renderPass, subpass);
		m_pipelineCreate.target_layout(pipelineLayout);

		auto _ctor = [&pipelineCreate = this->m_pipelineCreate](rhi::pmvPipeline* p) {
			VkResult result = pipelineCreate.publish(*p);
			if (result != VK_SUCCESS)
				throw std::runtime_error("FAILURE: pipeline_creation!");
		};

		return _wrt.construct(_ctor);
	}

}


