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
	const ctnr::vltView<renderTarget> m_targets;

	const ctnr::vltView<rhi::pmvRenderPass> a_renderpassView;
	const ctnr::vltView<rhi::pmvFramebuffer> a_framebufferView;
	const ctnr::vltView<rhi::pmvPipeline> a_pipelineView;

};