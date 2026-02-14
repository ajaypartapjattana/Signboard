#include "materials.h"

#include "Signboard/Assets/io/io.h"

#include "Signboard/Renderer/RenderBackend/rndr_interfaceAccess.h"

#include <vector>
#include <stdexcept>

materials::materials(const render_interface& ri)
	: m_device(rndr_interfaceAccess::say_device(ri)), m_swapchain(rndr_interfaceAccess::say_swapchain(ri))
{
	rhi::procedure::pipelineLayout_builder layout_builder{ m_device };
	layout_builder.build(m_pipelineLayout);
}

void materials::create_material(uint32_t targetPass_index, uint32_t subpass) {
	material baseMat{};

	baseMat.targetPass_index = targetPass_index;
	baseMat.targetSubpass_index = subpass;
	
	rhi::primitive::shader l_vertShader;
	create_shader(l_vertShader, "shaders/base.vert.spv");

	rhi::primitive::shader l_fragShader;
	create_shader(l_fragShader, "shaders/base.frag.spv");

	rhi::procedure::pipeline_builder pipeline_builder{ m_device, m_swapchain, m_pipelineLayout };
	pipeline_builder.set_vertShader(l_vertShader);
	pipeline_builder.set_fragShader(l_fragShader);
	pipeline_builder.set_targetPass();
	pipeline_builder.build_graphicsPipeline(0, baseMat.pipeline);

	storage::vault_writeAccessor<material> writer{m_vault};
	writer.create(baseMat);
	
}

void materials::create_shader(rhi::primitive::shader& tw_shader, const char* path) {
	char* spv_data = nullptr;
	size_t spv_size= 0;

	io::loader::file_loader loader{ &spv_data, &spv_size };
	if (loader.load_binary(path) != true)
		throw std::runtime_error("LOAD_FAILURE: " + std::string(path));

	rhi::procedure::shader_wrapper wrapper{ m_device };
	wrapper.set_target(tw_shader);
	if (wrapper.wrap_shaderCode(spv_data, spv_size) != VK_SUCCESS)
		throw std::runtime_error("FAILURE: shaderModule_creation!");
}