#include "materials.h"

#include "Signboard/Assets/io/io.h"


materials::materials(const rhi::creDevice& device, const rhi::pmvSwapchain& swapchain, ctnr::vltView<rhi::pmvRenderPass> passAccess, ctnr::vltView<rhi::pmvVertexLayout> fieldsAccess)
	: 
	r_device(device), 
	r_swapchain(swapchain),

	a_renderPassView(std::move(passAccess)),
	a_vertexLayoutView(std::move(fieldsAccess)),

	m_writeAccess(m_pipelines)
{
	rhi::pcdPipelineLayoutBuilder layout_builder{ r_device };
	layout_builder.build(m_pipelineLayout);
}

uint32_t materials::createPipeline(uint32_t renderPassIndex, uint32_t vertexLayoutHandle, uint32_t subpass, const createInfo& info) {
	rhi::pmvShader l_vertShader;
	createShader(l_vertShader, info.vertShader_path);

	rhi::pmvShader l_fragShader;
	createShader(l_fragShader, info.fragShader_path);

	rhi::pcdPipelineBuilder prcdr{ r_device, r_swapchain, m_pipelineLayout };
	prcdr.set_vertexLayout(a_vertexLayoutView.get(vertexLayoutHandle));

	prcdr.set_vertShader(l_vertShader);
	prcdr.set_fragShader(l_fragShader);

	prcdr.set_targetPass(a_renderPassView.get(renderPassIndex));

	auto builder = [&](rhi::pmvPipeline* p) {
		prcdr.build_graphicsPipeline(subpass, *p);
	};
	
	return m_writeAccess.construct(builder);
}

ctnr::vltView<rhi::pmvPipeline> materials::read_pipelines() const noexcept {
	return ctnr::vltView<rhi::pmvPipeline>{ m_pipelines };
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