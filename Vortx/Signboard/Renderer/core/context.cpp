#include "Signboard/Renderer/Internal/rndr_pAccess.h"
#include "Signboard/Platform/platform.h"

namespace rndr {

	creContext::creContext(const plf::displayWindow& window)
		:
		m_instance(describe_instance()),
		m_surface(window, m_instance),
		m_device(describe_device(), m_instance),
		m_allocator(m_instance, m_device)
	{

	}

	rhi::creInstance::createInfo creContext::describe_instance() noexcept {
		rhi::creInstance::createInfo info{};

		uint32_t ext_count = 0;
		const char** exts = plf::context::native_extensions(ext_count);

		info.extensions.reserve(ext_count);

		for (uint32_t i = 0; i < ext_count; ++i) {
			info.extensions.push_back(exts[i]);
		}

#ifdef _VALIDATE
		info.enable_validation = true;
#endif

		return info;
	}

	rhi::creDevice::createInfo creContext::describe_device() noexcept {
		rhi::creDevice::createInfo info{};

		info.present_surface = &m_surface;
		info.requiredFeatures = { &VkPhysicalDeviceFeatures::samplerAnisotropy };

		return info;
	}

}
