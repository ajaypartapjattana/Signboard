#include "window_eventState.h"

namespace platform::primitive {

	windowEventState::windowEventState() {

	}

	windowEventState* windowEventState::from(GLFWwindow* window) {
		return static_cast<windowEventState*>(glfwGetWindowUserPointer(window));
	}

}