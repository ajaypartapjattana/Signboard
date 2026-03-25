#pragma once

namespace platform::primitive {
	class displayWindow;
}

#include <vulkan/vulkan.h>

namespace rhi::access {
	struct surface_pAccess;
}

namespace rhi {

	class creInstance;

	class creSurface {
	public:
		creSurface(const platform::primitive::displayWindow& window, const creInstance& instance);

		creSurface(const creSurface&) = delete;
		creSurface& operator=(const creSurface&) = delete;

		creSurface(creSurface&&) noexcept;
		creSurface& operator=(creSurface&&) noexcept;

		~creSurface() noexcept;

	private:
		friend struct rhi::access::surface_pAccess;

		VkSurfaceKHR m_surface;
		VkInstance m_instance;

	};

}