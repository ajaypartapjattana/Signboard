#pragma once

namespace platform::primitive {
	class display_window;
	class window_eventState;
}

namespace platform::detail {
	class InputEvent;
}

struct InputEvent;

#include <GLFW/glfw3.h>
#include <vector>

namespace platform::procedure {

	class eventState_handler {
	public:
		eventState_handler(platform::primitive::window_eventState& state);

		void poll();
		void fetch_eventList(std::vector<platform::detail::InputEvent>& target_list);
		bool target_isAlive();

	private:
		platform::primitive::window_eventState& m_eventState;

	};

}