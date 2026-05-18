#pragma once

#include <vulkan/vulkan.h>

namespace plf {
	class displayWindow;
}

namespace rhi {

	struct _pAccess;

	class creInstance;

	class creSurface {
	public:
		creSurface(const plf::displayWindow& window, const creInstance& instance);

		creSurface(const creSurface&) = delete;
		creSurface& operator=(const creSurface&) = delete;

		creSurface(creSurface&&) noexcept;
		creSurface& operator=(creSurface&&) noexcept;

		~creSurface() noexcept;

	private:
		friend struct _pAccess;

		VkSurfaceKHR m_surface;
		VkInstance m_instance;

	};

}