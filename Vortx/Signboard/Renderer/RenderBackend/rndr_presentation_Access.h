#pragma once

#include "rndr_presentation.h"

struct rndr_presentation_Access {

	static const rhi::pmvSwapchain& get_swapchain(const rndr_presentation& p) noexcept {
		return p.m_swapchain;
	}

};