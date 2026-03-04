#pragma once

#include "rndr_context.h"

struct rndr_context_Access {

	static const rhi::core::instance& get_instance(const rndr_context& rc) noexcept {
		return rc.m_instance;
	}

	static const rhi::core::surface& get_surface(const rndr_context& rc) noexcept {
		return rc.m_surface;
	}

	static const rhi::core::device& get_device(const rndr_context& rc) noexcept {
		return rc.m_device;
	}

	static const rhi::core::allocator& get_allocator(const rndr_context& rc) noexcept {
		return rc.m_allocator;
	}

};