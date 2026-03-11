#include "rndr_context.h"

rndr_context::rndr_context(const platform::primitive::display_window& window)
	: m_instance(get_instanceCI()),
	m_surface(window, m_instance),
	m_device(get_deviceCI(), m_instance),
	m_allocator(m_instance, m_device)
{

}

rhi::core::instance_CI rndr_context::get_instanceCI() noexcept {
	rhi::core::instance_CI ci{};
	
	uint32_t ext_count = 0;
	const char** exts = platform::core::context::native_extensions(ext_count);

	ci.extensions.reserve(ext_count);

	for (uint32_t i = 0; i < ext_count; ++i) {
		ci.extensions.push_back(exts[i]);
	}

#ifndef NDEBUG
	ci.enable_validation = true;
#endif

	return ci;
}

rhi::core::device_CI rndr_context::get_deviceCI() noexcept {
	rhi::core::device_CI ci{};

	ci.requiredQueues = { VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_TRANSFER_BIT };
	ci.present_surface = &m_surface;
	ci.requiredFeatures = { &VkPhysicalDeviceFeatures::samplerAnisotropy };

	return ci;
}
