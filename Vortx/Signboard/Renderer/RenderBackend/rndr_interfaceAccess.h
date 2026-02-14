#pragma once

#include "render_interface.h"

struct rndr_interfaceAccess {

	static const rhi::core::device& say_device(const render_interface& ri) noexcept {
		return ri.m_device;
	}

	static const rhi::core::swapchain& say_swapchain(const render_interface& ri) noexcept {
		return ri.m_swapchain;
	}

	static const rhi::core::allocator& say_allocator(const render_interface& ri) noexcept {
		return ri.m_allocator;
	}

};