#include "display_window_builder.h"

#include "Signboard/Platform/window/primitive/display_window_glfwAccess.h"

#include <stdexcept>

namespace window::procedure {

	display_window_builder& display_window_builder::set_windowTitle(const std::string& title) {
		m_title = title;
		return *this;
	}
	
	display_window_builder& display_window_builder::setMode_windowed(const window_extent extent) {
		m_fullscreen = false;
		m_extent = extent;
		return *this;
	}

	display_window_builder& display_window_builder::setMode_Fullscreen() {
		m_fullscreen = true;
		return *this;
	}

	window::core::display_window display_window_builder::build() {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		GLFWmonitor* moniter = nullptr;

		if (m_fullscreen) {
			moniter = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(moniter);

			m_extent.width = mode->width;
			m_extent.height = mode->height;
		}
			

		GLFWwindow* glfw_window = glfwCreateWindow(m_extent.width, m_extent.height, m_title.c_str(), moniter, nullptr);

		if (!glfw_window)
			throw std::runtime_error("failed to create display window!");

		window::core::display_window l_window;
		l_window.m_window = glfw_window;
		
		return l_window;
	}

}