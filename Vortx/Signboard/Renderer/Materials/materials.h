#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Core/Containers/storage.h"

class passes;

class materials {
public:
	materials(const rhi::creDevice& device, const rhi::pmvSwapchain& swapchain, ctnr::vltView<rhi::pmvRenderPass> passAccess, ctnr::vltView<rhi::pmvVertexLayout> fieldsAccess);

	struct createInfo {
		const char* vertShader_path;
		const char* fragShader_path;
	};

	uint32_t createPipeline(uint32_t renderPassIndex, uint32_t vertexLayoutHandle, uint32_t subpass, const createInfo& createInfo);
	ctnr::vltView<rhi::pmvPipeline> read_pipelines() const noexcept;

private:
	VkResult createShader(rhi::pmvShader& tw_shader, const char* path);

private:
	const rhi::creDevice& r_device;
	const rhi::pmvSwapchain& r_swapchain;

	const ctnr::vltView<rhi::pmvRenderPass> a_renderPassView;
	const ctnr::vltView<rhi::pmvVertexLayout> a_vertexLayoutView;

	rhi::pmvPipelineLayout m_pipelineLayout;

	ctnr::vault<rhi::pmvPipeline> m_pipelines;
	ctnr::vault_writeAccessor<rhi::pmvPipeline> m_writeAccess;

};