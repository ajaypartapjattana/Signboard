#include "render_interface.h"

render_interface::render_interface(const platform::primitive::display_window& a_window) 
	: m_instance(setup_instance()),
	m_surface(setup_surface(a_window)),

	m_device(setup_device()),

	m_swapchain(setup_swapchain()),
	m_allocator(setup_allocator())
{
	rhi::procedure::commandPool_creator l_creator{ m_device };

	uint32_t poolCount = l_creator.get_requiredPoolCount();

	m_commandPools.resize(poolCount);
	l_creator.create(m_commandPools.data(), poolCount);
}

rhi::core::instance render_interface::setup_instance() {
	rhi::procedure::instance_builder l_builder;

	uint32_t ext_count = 0;
	const char** exts = platform::core::context::native_extensions(ext_count);

	for (uint32_t i = 0; i < ext_count; ++i)
		l_builder.addExtension(exts[i]);

#ifndef NDEBUG
	l_builder.enableValidationLayer();
	l_builder.addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	return l_builder.build();	
}

rhi::core::surface render_interface::setup_surface(const platform::primitive::display_window& window) {
	rhi::procedure::surface_creator l_creator{ m_instance };

	return l_creator.create_surface(window);
}

rhi::core::device render_interface::setup_device() {
	rhi::procedure::device_builder l_builder{ m_instance };

	l_builder.require_graphicsQueue();
	l_builder.require_presentQueue(m_surface);

	l_builder.enable_samplerAnisotropy();

	return l_builder.build();
}

rhi::core::swapchain render_interface::setup_swapchain() {
	rhi::procedure::swapchain_builder l_builder{ m_device, m_surface };

	l_builder.prefer_format_srgb();
	l_builder.prefer_presentMode_MAILBOX();

	l_builder.set_imageCount(2);

	return l_builder.build();
}

rhi::core::allocator render_interface::setup_allocator() {
	rhi::procedure::allocator_creator l_creator{ m_instance, m_device };

	return l_creator.create();
}