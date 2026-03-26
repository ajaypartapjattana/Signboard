#include "materials.h"

#include "Signboard/Assets/io/io.h"

#include "Signboard/Renderer/Pass/passes_Access.h"

#include <vector>
#include <stdexcept>

materials::materials(const rhi::creDevice& device, const rhi::pmvSwapchain& swapchain, const passes& passes)
	: 
	r_device(device), 
	r_swapchain(swapchain),
	a_renderPass_RAccess(passes.get_rp_readAccessor()),

	m_writeAccess(m_pipelines)
{
	rhi::pcdPipelineLayoutBuilder layout_builder{ r_device };
	layout_builder.build(m_pipelineLayout);
}

storage::storage_handle materials::create_pipeline(storage::storage_handle passHandle, uint32_t subpass, const createInfo* info) {
	rhi::pmvShader l_vertShader;
	createShader(l_vertShader, info->vertShader_path);

	rhi::pmvShader l_fragShader;
	createShader(l_fragShader, info->fragShader_path);

	rhi::pcdPipelineBuilder prcdr{ r_device, r_swapchain, m_pipelineLayout };
	prcdr.set_vertShader(l_vertShader);
	prcdr.set_fragShader(l_fragShader);

	prcdr.set_targetPass(*a_renderPass_RAccess.get(passHandle));

	auto builder = [&](rhi::pmvPipeline* p) {
		prcdr.build_graphicsPipeline(subpass, *p);
	};
	
	return m_writeAccess.construct(builder);
}

storage::vault_readAccessor<rhi::pmvPipeline> materials::get_readAccessor() const noexcept {
	storage::vault_readAccessor<rhi::pmvPipeline> accessor{m_pipelines};
	return accessor;
}

VkResult materials::createShader(rhi::pmvShader& tw_shader, const char* path) {
	char* spv_data = nullptr;
	size_t spv_size= 0;

	io::loader::file_loader loader{ &spv_data, &spv_size };
	if (!loader.load_binary(path))
		return VK_INCOMPLETE;

	rhi::pcdShaderWrapper wrapper{ r_device };
	wrapper.addBinary(spv_data, spv_size);

	return wrapper.warpShaderCode(tw_shader);
}