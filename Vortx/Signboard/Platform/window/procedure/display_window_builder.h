#pragma once

#include <string>
#include <cstdint>

namespace window::core {
	class display_window;
}

namespace window::procedure {

	class display_window_builder {
	public:

		display_window_builder& set_windowTitle(const std::string& title);
		
		struct window_extent {
			uint32_t width;
			uint32_t height;
		};
		display_window_builder& setMode_windowed(const window_extent extent);
		display_window_builder& setMode_Fullscreen();

		window::core::display_window build();

	private:
		window_extent m_extent;
		std::string& m_title;

		bool m_fullscreen = false;

	};

}