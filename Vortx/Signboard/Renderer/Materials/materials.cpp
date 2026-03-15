#include "materials.h"

#include "Signboard/Assets/io/io.h"

#include "Signboard/Renderer/Pass/passes_Access.h"

#include <vector>
#include <stdexcept>

materials::materials(const rhi::core::device& device, const rhi::primitive::swapchain& swapchain, const passes& passes)
	: 
	r_device(device), 
	r_swapchain(swapchain),
	a_renderPass_RAccess(passes.get_readAccessor()),

	m_writeAccess(m_pipelines)
{
	rhi::procedure::pipelineLayout_builder layout_builder{ r_device };
	layout_builder.build(m_pipelineLayout);
}

storage::storage_handle materials::create_pipeline(storage::storage_handle passHandle, uint32_t subpass, const createInfo* info) {
	rhi::primitive::shader l_vertShader;
	create_shader(l_vertShader, info->vertShader_path);

	rhi::primitive::shader l_fragShader;
	create_shader(l_fragShader, info->fragShader_path);

	rhi::procedure::pipeline_builder prcdr{ r_device, r_swapchain, m_pipelineLayout };
	prcdr.set_vertShader(l_vertShader);
	prcdr.set_fragShader(l_fragShader);

	prcdr.set_targetPass(*a_renderPass_RAccess.get(passHandle));

	auto builder = [&](rhi::primitive::pipeline* p) {
		prcdr.build_graphicsPipeline(subpass, *p);
	};
	
	return m_writeAccess.construct(builder);
}

storage::vault_readAccessor<rhi::primitive::pipeline> materials::get_readAccessor() const noexcept {
	storage::vault_readAccessor<rhi::primitive::pipeline> accessor{m_pipelines};
	return accessor;
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