#include "eventState_initializer.h"

#include "Signboard/Platform/primitive/window_eventState_pAccess.h"
#include "Signboard/Platform/primitive/display_window_glfwAccess.h"

namespace {

	void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		if (auto* state = static_cast<platform::primitive::window_eventState*>(glfwGetWindowUserPointer(window)))
			platform::primitive::window_eventState_pAccess::getf_framebufferResize(*state)(*state, width, height);
	}

	void fileDropCallback(GLFWwindow* window, int count, const char** paths) {
		if (auto* state = static_cast<platform::primitive::window_eventState*>(glfwGetWindowUserPointer(window)))
			platform::primitive::window_eventState_pAccess::getf_fileDrop(*state)(*state, count, paths);
	}

	void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
		if (auto* state = static_cast<platform::primitive::window_eventState*>(glfwGetWindowUserPointer(window)))
			platform::primitive::window_eventState_pAccess::getf_keyCallback(*state)(*state, key, scanCode, action, mods);
	}

	void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
		if (auto* state = static_cast<platform::primitive::window_eventState*>(glfwGetWindowUserPointer(window)))
			platform::primitive::window_eventState_pAccess::getf_scrollCallback(*state)(*state, xOffset, yOffset);
	}

	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		if (auto* state = static_cast<platform::primitive::window_eventState*>(glfwGetWindowUserPointer(window)))
			platform::primitive::window_eventState_pAccess::getf_mouseButtonCallback(*state)(*state, button, action, mods);
	}

	void cursorMoveCallback(GLFWwindow* window, double xPos, double yPos) {
		if(auto* state = static_cast<platform::primitive::window_eventState*>(glfwGetWindowUserPointer(window)))
			platform::primitive::window_eventState_pAccess::getf_cursorMoveCallback(*state)(*state, xPos, yPos);
	}

}

namespace platform::procedure {

	eventState_initializer::eventState_initializer(platform::primitive::window_eventState& es)
		: m_eventState(es)
	{

	}

	eventState_initializer& eventState_initializer::setFramebufferResizeCallback(std::function<void(platform::primitive::window_eventState&, int width, int height)> cb) {
		m_eventState.s_onFramebufferResize = std::move(cb);
		return *this;
	}

	eventState_initializer& eventState_initializer::setFileDropCallback(std::function<void(platform::primitive::window_eventState&, int count, const char** paths)> cb) {
		m_eventState.s_onFileDrop = std::move(cb);
		return *this;
	}

	eventState_initializer& eventState_initializer::set_keyCallback(std::function<void(platform::primitive::window_eventState&, int, int, int, int)> cb) {
		m_eventState.s_keyCallback = std::move(cb);
		return *this;
	}

	eventState_initializer& eventState_initializer::set_scrollCallback(std::function<void(platform::primitive::window_eventState&, double, double)> cb) {
		m_eventState.s_scrollCallback = std::move(cb);
		return *this;
	}

	eventState_initializer& eventState_initializer::set_mouseButtonCallback(std::function<void(platform::primitive::window_eventState&, int, int, int)> cb) {
		m_eventState.s_mouseButtonCallback = std::move(cb);
		return *this;
	}

	eventState_initializer& eventState_initializer::set_cursorMoveCallback(std::function<void(platform::primitive::window_eventState&, double, double)> cb) {
		m_eventState.s_cursorMoveCallback = std::move(cb);
		return *this;
	}

	void eventState_initializer::attach(const platform::primitive::display_window& window) {
		GLFWwindow* a_window = platform::primitive::display_window_glfwAccess::get(window);

		m_eventState.m_window = a_window;
		glfwSetWindowUserPointer(a_window, &m_eventState);

		glfwSetFramebufferSizeCallback(a_window, framebufferResizeCallback);
		glfwSetDropCallback(a_window, fileDropCallback);

		glfwSetKeyCallback(a_window, keyCallback);
		glfwSetScrollCallback(a_window, scrollCallback);
		glfwSetMouseButtonCallback(a_window, mouseButtonCallback);
		glfwSetCursorPosCallback(a_window, cursorMoveCallback);
	}

}