#pragma once

#include "rndr_interface.h"

struct rndr_interface_Access {

	static const rhi::pmvSwapchain& get_swapchain(const rndr_interface& ri) noexcept {
		return ri.r_swapchain;
	}

};