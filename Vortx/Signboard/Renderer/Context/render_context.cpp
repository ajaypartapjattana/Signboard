#include "render_context.h"

RHIContext::RHIContext(const platform::primitive::displayWindow& window)
	: m_instance(get_instanceCreateInfo()),
	m_surface(window, m_instance),
	_dvc(get_deviceCreateInfo(), m_instance),
	_allctr(m_instance, _dvc)
{

}

rhi::creInstance::createInfo RHIContext::get_instanceCreateInfo() noexcept {
	rhi::creInstance::createInfo info{};
	
	uint32_t ext_count = 0;
	const char** exts = platform::core::context::native_extensions(ext_count);

	info.extensions.reserve(ext_count);

	for (uint32_t i = 0; i < ext_count; ++i) {
		info.extensions.push_back(exts[i]);
	}

#ifdef _VALIDATE
	info.enable_validation = true;
#endif

	return info;
}

rhi::creDevice::createInfo RHIContext::get_deviceCreateInfo() noexcept {
	rhi::creDevice::createInfo info{};

	info.present_surface = &m_surface;
	info.requiredFeatures = { &VkPhysicalDeviceFeatures::samplerAnisotropy };

	return info;
}
