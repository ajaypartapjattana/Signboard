#pragma once

namespace platform::core {
	class context;
}

namespace platform::primitive {
	class display_window;
}

#include <string>
#include <cstdint>

namespace platform::procedure {

	class display_window_builder {
	public:
		display_window_builder(const platform::core::context& ctx);

		display_window_builder& set_windowTitle( std::string& title);
		
		struct window_extent {
			uint32_t width;
			uint32_t height;
		};
		display_window_builder& setMode_windowed(const window_extent extent);
		display_window_builder& setMode_Fullscreen();

		platform::primitive::display_window build();

	private:
		window_extent m_extent = { 0, 0 };
		std::string* m_title = nullptr;

		bool m_fullscreen = false;

	};

}