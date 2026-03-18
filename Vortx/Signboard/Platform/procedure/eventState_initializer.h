#pragma once

namespace platform::primitive {
	class display_window;
	class window_eventState;
}

#include <GLFW/glfw3.h>
#include <functional>

namespace platform::procedure {

	class eventState_initializer {
	public:
		eventState_initializer(const platform::primitive::display_window& target_window);

		void attach(platform::primitive::window_eventState& eventState) const;
		void initiate_callbacks() const;

	private:
		GLFWwindow* r_window;

	};

}