#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Platform/platform.h"

struct rndr_context_Access;

class rndr_context {
public:
	rndr_context(const platform::primitive::display_window& window);

private:
	rhi::core::instance setup_instance();
	rhi::core::surface setup_surface(const platform::primitive::display_window& window);

	rhi::core::device setup_device();

	rhi::core::allocator setup_allocator();

private:
	friend struct rndr_context_Access;

	rhi::core::instance m_instance;
	rhi::core::surface m_surface;

	rhi::core::device m_device;

	rhi::core::allocator m_allocator;

};