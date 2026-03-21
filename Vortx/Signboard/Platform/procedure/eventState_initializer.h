#pragma once

namespace platform::primitive {
	class displayWindow;
	class windowEventState;
}

#include <GLFW/glfw3.h>

namespace platform::procedure {

	class eventState_initializer {
	public:
		eventState_initializer(const platform::primitive::displayWindow& target_window);

		void attach(platform::primitive::windowEventState& eventState) const;
		void initiate_callbacks() const;

	private:
		GLFWwindow* r_window;

	};

}