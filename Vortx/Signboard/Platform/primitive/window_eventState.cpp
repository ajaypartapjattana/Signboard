#include "window_eventState.h"

#include "display_window_glfwAccess.h"

namespace platform::primitive {

	window_eventState::window_eventState() {

	}

	window_eventState* window_eventState::from(GLFWwindow* window) {
		return static_cast<window_eventState*>(glfwGetWindowUserPointer(window));
	}

	bool window_eventState::window_isAlive() const {
		return !glfwWindowShouldClose(m_window);
	}

}