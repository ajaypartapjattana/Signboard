#pragma once

#include "Signboard/RHI/rhi.h"
#include "rndr_method.h"

class rndr_framedraw {
public:
	rndr_framedraw(const rndr_method& methods);

	rndr_framedraw(const rndr_framedraw&) = delete;
	rndr_framedraw& operator=(const rndr_framedraw&) = delete;

	void drawFrame(const uint32_t target_index, const rhi::primitive::commandBuffer& target_cmd);

private:
	const storage::vault_readAccessor<render_target> m_targets;

	const storage::vault_readAccessor<rhi::primitive::renderPass> a_renderpassView;
	const storage::vault_readAccessor<rhi::primitive::framebuffer> a_framebufferView;
	const storage::vault_readAccessor<rhi::primitive::pipeline> a_pipelineView;

};