#pragma once

namespace platform::primitive {
	class display_window;
}

#include <vulkan/vulkan.h>

namespace rhi {

	namespace core {
		class instance;
		class surface;
	}

	namespace procedure {

		class surface_creator {
		public:
			surface_creator(const core::instance& instance);

			surface_creator(const surface_creator&) = delete;
			surface_creator& operator=(const surface_creator&) = delete;

			core::surface create_surface(const platform::primitive::display_window& window);

		private:
			VkInstance m_instance;

		};


	}

}