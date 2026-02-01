#pragma once

#include "Signboard/Platform/platform.h"
#include "Signboard/RHI/rhi.h"

class render_interface {
public:
	render_interface(const platform::primitive::display_window& a_window);

private:
	rhi::core::instance setup_instance();
	rhi::core::surface setup_surface(const platform::primitive::display_window& window);

	rhi::core::device setup_device();

	rhi::core::swapchain setup_swapchain();
	rhi::core::allocator setup_allocator();

private:
	rhi::core::instance m_instance;
	rhi::core::surface m_surface;

	rhi::core::device m_device;

	rhi::core::swapchain m_swapchain;
	rhi::core::allocator m_allocator;

};