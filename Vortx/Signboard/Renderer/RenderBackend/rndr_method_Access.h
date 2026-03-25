#pragma once

#include "rndr_method.h"

struct rndr_method_Access {

	static const storage::vault_readAccessor<rhi::pmvRenderPass> get_renderPassView(const rndr_method& m) noexcept {
		return m.m_passes.get_rp_readAccessor();
	}

	static const storage::vault_readAccessor<rhi::pmvFramebuffer> get_framebufferView(const rndr_method& m) noexcept {
		return m.m_passes.get_fb_readAccessor();
	}

	static const storage::vault_readAccessor<rhi::pmvPipeline> get_pipelineView(const rndr_method& m) noexcept {
		return m.m_materials.get_readAccessor();
	}

};