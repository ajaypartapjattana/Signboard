#include "materials.h"

#include "Signboard/Assets/io/io.h"

#include "Signboard/Renderer/RenderBackend/rndr_context_Access.h"
#include "Signboard/Renderer/RenderBackend/rndr_presentation_Access.h"
#include "Signboard/Renderer/Pass/passes_Access.h"

#include <vector>
#include <stdexcept>

materials::materials(const rndr_context& context, const rndr_presentation& presentation, const passes& passes)
	: 
	r_device(rndr_context_Access::get_device(context)), 
	r_swapchain(rndr_presentation_Access::get_swapchain(presentation)), 
	r_passes(passes)
{
	rhi::procedure::pipelineLayout_builder layout_builder{ r_device };
	layout_builder.build(m_pipelineLayout);
}

void materials::create_baseMaterial(uint32_t targetPass_index, uint32_t subpass) {

	m_baseMat.targetPass_index = targetPass_index;
	m_baseMat.targetSubpass_index = subpass;

	const rhi::primitive::renderPass& a_pass = passes_Access::get_primiaryPass(r_passes);
	
	rhi::primitive::shader l_vertShader;
	create_shader(l_vertShader, "shaders/base.vert.spv");

	rhi::primitive::shader l_fragShader;
	create_shader(l_fragShader, "shaders/base.frag.spv");

	rhi::procedure::pipeline_builder pipeline_builder{ r_device, r_swapchain, m_pipelineLayout };
	pipeline_builder.set_vertShader(l_vertShader);
	pipeline_builder.set_fragShader(l_fragShader);
	pipeline_builder.set_targetPass(a_pass);
	pipeline_builder.build_graphicsPipeline(0, m_baseMat.pipeline);


	/*storage::vault_writeAccessor<material> writer{ m_vault };
	writer.create(baseMat);*/
	
}

void materials::create_shader(rhi::primitive::shader& tw_shader, const char* path) {
	char* spv_data = nullptr;
	size_t spv_size= 0;

	io::loader::file_loader loader{ &spv_data, &spv_size };
	if (loader.load_binary(path) != true)
		throw std::runtime_error("LOAD_FAILURE: " + std::string(path));

	rhi::procedure::shader_wrapper wrapper{ r_device };
	wrapper.set_target(tw_shader);
	if (wrapper.wrap_shaderCode(spv_data, spv_size) != VK_SUCCESS)
		throw std::runtime_error("FAILURE: shaderModule_creation!");
}