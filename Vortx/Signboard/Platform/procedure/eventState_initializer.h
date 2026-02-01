#pragma once

namespace platform::primitive {
	class display_window;
	class window_eventState;
}

#include <functional>

namespace platform::procedure {

	class eventState_initializer {
	public:
		eventState_initializer(platform::primitive::window_eventState& eventState);

		eventState_initializer& setFramebufferResizeCallback(std::function<void(platform::primitive::window_eventState&, int width, int height)> cbf);
		eventState_initializer& setFileDropCallback	(std::function<void(platform::primitive::window_eventState&, int count, const char** path)> cbf);

		eventState_initializer& set_keyCallback(std::function<void(platform::primitive::window_eventState&, int, int, int, int)> cbf);
		eventState_initializer& set_scrollCallback(std::function<void(platform::primitive::window_eventState&, double, double)> cbf);
		eventState_initializer& set_mouseButtonCallback(std::function<void(platform::primitive::window_eventState&, int, int, int)> cbf);
		eventState_initializer& set_cursorMoveCallback(std::function<void(platform::primitive::window_eventState&, double, double)> cbf);

		void attach(const platform::primitive::display_window& window);

	private:
		platform::primitive::window_eventState& m_eventState;

	};

}