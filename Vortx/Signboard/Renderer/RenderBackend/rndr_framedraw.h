#pragma once

#include "Signboard/RHI/rhi.h"
#include "rndr_method.h"

class rndr_framedraw {
public:
	rndr_framedraw(const rndr_method& methods);

	rndr_framedraw(const rndr_framedraw&) = delete;
	rndr_framedraw& operator=(const rndr_framedraw&) = delete;

	void drawFrame(const uint32_t target_index, const rhi::pmvCommandBuffer& target_cmd);

private:
	const storage::vault_readAccessor<render_target> m_targets;

	const storage::vault_readAccessor<rhi::pmvRenderPass> a_renderpassView;
	const storage::vault_readAccessor<rhi::pmvFramebuffer> a_framebufferView;
	const storage::vault_readAccessor<rhi::pmvPipeline> a_pipelineView;

};