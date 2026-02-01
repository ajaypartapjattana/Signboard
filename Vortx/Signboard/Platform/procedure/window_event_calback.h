#pragma once

namespace platform::core {
	class window_eventState;
}

#include <GLFW/glfw3.h>

namespace platform::procedure {

	class window_event_callback {
	public:
		void attach(GLFWwindow* window, platform::core::window_eventState* state);

	};

}