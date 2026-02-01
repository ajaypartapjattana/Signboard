#pragma once

namespace platform::primitive {
	class display_window;
	class window_eventState;
}

struct InputEvent;

#include <GLFW/glfw3.h>
#include <vector>

namespace platform::procedure {

	class window_eventHandler {
	public:
		window_eventHandler(platform::primitive::window_eventState& state);

		void poll();
		void fetch_eventList(std::vector<InputEvent>& target_list);
		bool target_isAlive();

	private:
		platform::primitive::window_eventState& m_eventState;

	};

}