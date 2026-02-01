#include "display_window.h"

namespace platform::primitive {

	display_window::display_window(display_window&& other) noexcept {
		m_window = other.m_window;
		other.m_window = nullptr;
	}

	display_window& display_window::operator=(display_window&& other) noexcept {
		if (m_window)
			glfwDestroyWindow(m_window);

		m_window = other.m_window;
		other.m_window = nullptr;
	}

	display_window::~display_window() noexcept {
		if (m_window) {
			glfwDestroyWindow(m_window);
		}
	}

	const GLFWwindow* display_window::native_window() const noexcept {
		return m_window;
	}

}