#include "rndr_context.h"

rndr_context::rndr_context(const platform::primitive::display_window& window)
	: m_instance(setup_instance()),
	m_surface(setup_surface(window)), 
	m_device(setup_device()), 
	m_allocator(setup_allocator())
{

}

rhi::core::instance rndr_context::setup_instance() {
	rhi::procedure::instance_builder l_builder;

	uint32_t ext_count = 0;
	const char** exts = platform::core::context::native_extensions(ext_count);

	for (uint32_t i = 0; i < ext_count; ++i) {
		l_builder.addExtension(exts[i]);
	}

#ifndef NDEBUG
	l_builder.enableValidationLayer();
	l_builder.addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	return l_builder.build();
}

rhi::core::surface rndr_context::setup_surface(const platform::primitive::display_window& window) {
	rhi::procedure::surface_creator l_creator{ m_instance };

	return l_creator.create_surface(window);
}

rhi::core::device rndr_context::setup_device() {
	rhi::procedure::device_builder l_builder{ m_instance };

	l_builder.require_graphicsQueue();
	l_builder.require_transferQueue();
	l_builder.require_presentQueue(m_surface);

	l_builder.enable_samplerAnisotropy();

	return l_builder.build();
}

rhi::core::allocator rndr_context::setup_allocator() {
	rhi::procedure::allocator_creator l_creator{ m_instance, m_device };

	return l_creator.create();
}