#pragma once

namespace rhi::core {
	class instance;
	class surface;
}

namespace platform::primitive {
	class display_window;
}

#include <vulkan/vulkan.h>

namespace rhi::procedure {

	class surface_creator {
	public:
		surface_creator(const core::instance& instance);

		surface_creator(const surface_creator&) = delete;
		surface_creator& operator=(const surface_creator&) = delete;

		VkResult create_surface(const platform::primitive::display_window& window, rhi::core::surface& target_surface) const;

	private:
		VkInstance m_instance;

	};

}