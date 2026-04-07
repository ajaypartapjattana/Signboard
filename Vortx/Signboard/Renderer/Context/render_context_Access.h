#pragma once

#include "render_context.h"

struct rndr_context_Access {

	static const rhi::creInstance& get_instance(const RHIContext& rc) noexcept {
		return rc.m_instance;
	}

	static const rhi::creSurface& get_surface(const RHIContext& rc) noexcept {
		return rc.m_surface;
	}

	static const rhi::creDevice& get_device(const RHIContext& rc) noexcept {
		return rc._dvc;
	}

	static const rhi::creAllocator& get_allocator(const RHIContext& rc) noexcept {
		return rc._allctr;
	}

};