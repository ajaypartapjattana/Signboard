#include "rndr_context.h"

rndr_context::rndr_context(const platform::primitive::displayWindow& window)
	: m_instance(get_instanceCreateInfo()),
	m_surface(window, m_instance),
	m_device(get_deviceCreateInfo(), m_instance),
	m_allocator(m_instance, m_device)
{

}

rhi::core::instance::createInfo rndr_context::get_instanceCreateInfo() noexcept {
	rhi::core::instance::createInfo info{};
	
	uint32_t ext_count = 0;
	const char** exts = platform::core::context::native_extensions(ext_count);

	info.extensions.reserve(ext_count);

	for (uint32_t i = 0; i < ext_count; ++i) {
		info.extensions.push_back(exts[i]);
	}

#ifndef NDEBUG
	info.enable_validation = true;
#endif

	return info;
}

rhi::core::device::createInfo rndr_context::get_deviceCreateInfo() noexcept {
	rhi::core::device::createInfo info{};

	info.present_surface = &m_surface;
	info.requiredFeatures = { &VkPhysicalDeviceFeatures::samplerAnisotropy };

	return info;
}
