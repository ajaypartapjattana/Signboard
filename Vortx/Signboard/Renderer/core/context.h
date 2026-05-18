#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Platform/platform.h"

namespace plf {
	class displayWindow;
}

namespace rndr {

	struct _pAccess;

	class creContext {
	public:
		creContext(const plf::displayWindow& window);

		creContext(const creContext&) = delete;
		creContext& operator=(const creContext&) = delete;

	private:
		rhi::creInstance::createInfo describe_instance() noexcept;
		rhi::creDevice::createInfo describe_device() noexcept;

	private:
		friend struct _pAccess;

		rhi::creInstance m_instance;
		rhi::creSurface m_surface;

		rhi::creDevice m_device;

		rhi::creAllocator m_allocator;

	};

}
