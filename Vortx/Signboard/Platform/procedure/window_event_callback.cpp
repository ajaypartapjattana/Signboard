#include "window_event_calback.h"

#include "Signboard/Platform/core/eventState.h"

namespace {
	void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto* state = static_cast<platform::core::window_eventState*>(glfwGetWindowUserPointer(window));

		if (state && state->onFramebufferResize)
			state->onFramebufferResize(state->user, width, height);
	}

	void fileDropCallback(GLFWwindow* window, int count, const char** path) {
		auto* state = static_cast<platform::core::window_eventState*>(glfwGetWindowUserPointer(window));

		if (state && state->onFileDrop)
			state->onFileDrop(state->user, count, path);
	}
}

namespace platform::procedure {

	void window_event_callback::attach(GLFWwindow* window, platform::core::window_eventState* state) {
		glfwSetWindowUserPointer(window, state);

		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		glfwSetDropCallback(window, fileDropCallback);
	}

}