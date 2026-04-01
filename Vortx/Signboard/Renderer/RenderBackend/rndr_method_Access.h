#pragma once

#include "rndr_method.h"

struct rndr_method_Access {

	static const ctnr::vltView<rhi::pmvRenderPass> get_renderPassView(const rndr_method& m) noexcept {
		return m.m_passes.read_renderPasses();
	}

	static const ctnr::vltView<rhi::pmvFramebuffer> get_framebufferView(const rndr_method& m) noexcept {
		return m.m_passes.read_framebuffers();
	}

	static const ctnr::vltView<rhi::pmvPipeline> get_pipelineView(const rndr_method& m) noexcept {
		return m.m_materials.read_renderTargets();
	}

};