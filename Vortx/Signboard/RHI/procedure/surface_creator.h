#pragma once

namespace window::primitive {
	class display_window;
}

namespace rhi {

	namespace core {
		class instance;
		class surface;
	}

	namespace procedure {

		class surface_creator {
		public:
			explicit surface_creator(const core::instance& instance);

			surface_creator(const surface_creator&) = delete;
			surface_creator& operator=(const surface_creator&) = delete;

			core::surface create_surface(const window::primitive::display_window& window);

		private:
			const core::instance& m_instance;

		};


	}

}