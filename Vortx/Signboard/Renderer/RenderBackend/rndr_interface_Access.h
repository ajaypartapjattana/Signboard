#pragma once

#include "rndr_interface.h"

struct rndr_interface_Access {

	static const rhi::primitive::swapchain& get_swapchain(const rndr_interface& ri) noexcept {
		return ri.m_swapchain;
	}

};