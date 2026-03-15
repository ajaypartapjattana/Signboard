#pragma once

class rndr_context;
class rndr_presentation;

#include "Signboard/RHI/rhi.h"
#include "rndr_method.h"

class rndr_framedraw {
public:
	rndr_framedraw(const rndr_context& context, const rndr_presentation& presentation, const rndr_method& methods);

	rndr_framedraw(const rndr_framedraw&) = delete;
	rndr_framedraw& operator=(const rndr_framedraw&) = delete;

	void drawFrame(const uint32_t target_index, const rhi::primitive::commandBuffer& target_cmd);

private:
	const rhi::core::device& r_device;
	const rhi::primitive::swapchain& r_swapchain;

	const storage::vault_readAccessor<render_target> m_targets;

	const storage::vault_readAccessor<rhi::primitive::renderPass> a_renderpassView;
	const storage::vault_readAccessor<rhi::primitive::framebuffer> a_framebufferView;
	const storage::vault_readAccessor<rhi::primitive::pipeline> a_pipelineView;

	uint32_t bufferedFrame_count = 2;
	std::vector<rhi::primitive::framebuffer> m_primaryFramebuffers;


};