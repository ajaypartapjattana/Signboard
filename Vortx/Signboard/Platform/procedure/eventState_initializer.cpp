#include "eventState_initializer.h"

#include "Signboard/Platform/primitive/window_eventState.h"
#include "Signboard/Platform/primitive/display_window_glfwAccess.h"

#include "Signboard/Platform/detail/glfw_callbacks.h"

namespace platform::procedure {

	eventState_initializer::eventState_initializer(const platform::primitive::displayWindow& window)
		:
		r_window(platform::primitive::displayWindow_pAccess::get(window))
	{
		
	}

	void eventState_initializer::attach(platform::primitive::windowEventState& eventState) const {
		eventState.m_window = r_window;
		glfwSetWindowUserPointer(r_window, &eventState);
	}

	void eventState_initializer::initiate_callbacks() const {
		glfwSetFramebufferSizeCallback(r_window, platform::detail::glfw_callbacks::framebufferResizeCallback);
		glfwSetDropCallback(r_window, platform::detail::glfw_callbacks::fileDropCallback);
		glfwSetKeyCallback(r_window, platform::detail::glfw_callbacks::keyCallback);
		glfwSetScrollCallback(r_window, platform::detail::glfw_callbacks::scrollCallback);
		glfwSetMouseButtonCallback(r_window, platform::detail::glfw_callbacks::mouseButtonCallback);
		glfwSetCursorPosCallback(r_window, platform::detail::glfw_callbacks::cursorMoveCallback);
	}

}