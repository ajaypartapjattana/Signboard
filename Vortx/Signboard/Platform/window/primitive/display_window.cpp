#include "display_window.h"

#include <GLFW/glfw3.h>

namespace window::core {

	display_window::display_window(display_window&& other) noexcept {
		m_window = other.m_window;
		other.m_window = nullptr;
	}

	display_window& display_window::operator=(display_window&& other) noexcept {
		glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_window));

		m_window = other.m_window;
		other.m_window = nullptr;
	}

	display_window::~display_window() noexcept {
		if (m_window) {
			glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_window));
			m_window = nullptr;
		}
	}

	const void* display_window::native_window() const noexcept {
		return m_window;
	}

}