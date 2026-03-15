#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Core/Containers/storage/storage.h"

class passes;

class materials {
public:
	materials(const rhi::core::device& device, const rhi::primitive::swapchain& swapchain, const passes& passes);

	struct createInfo {
		const char* vertShader_path;
		const char* fragShader_path;
	};

	storage::storage_handle create_pipeline(storage::storage_handle passHandle, uint32_t subpass, const createInfo* createInfo);
	storage::vault_readAccessor<rhi::primitive::pipeline> get_readAccessor() const noexcept;

private:
	void create_shader(rhi::primitive::shader& tw_shader, const char* path);

private:
	const rhi::core::device& r_device;
	const rhi::primitive::swapchain& r_swapchain;
	const storage::vault_readAccessor<rhi::primitive::renderPass> a_renderPass_RAccess;

	rhi::primitive::pipelineLayout m_pipelineLayout;

	storage::vault<rhi::primitive::pipeline> m_pipelines;
	storage::vault_writeAccessor<rhi::primitive::pipeline> m_writeAccess;

};