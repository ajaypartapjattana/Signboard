#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Core/Containers/storage/storage.h"

class passes;

class materials {
public:
	materials(const rhi::creDevice& device, const rhi::pmvSwapchain& swapchain, const passes& passes);

	struct createInfo {
		const char* vertShader_path;
		const char* fragShader_path;
	};

	storage::storage_handle create_pipeline(storage::storage_handle passHandle, uint32_t subpass, const createInfo* createInfo);
	storage::vault_readAccessor<rhi::pmvPipeline> get_readAccessor() const noexcept;

private:
	void create_shader(rhi::pmvShader& tw_shader, const char* path);

private:
	const rhi::creDevice& r_device;
	const rhi::pmvSwapchain& r_swapchain;
	const storage::vault_readAccessor<rhi::pmvRenderPass> a_renderPass_RAccess;

	rhi::pmvPipelineLayout m_pipelineLayout;

	storage::vault<rhi::pmvPipeline> m_pipelines;
	storage::vault_writeAccessor<rhi::pmvPipeline> m_writeAccess;

};