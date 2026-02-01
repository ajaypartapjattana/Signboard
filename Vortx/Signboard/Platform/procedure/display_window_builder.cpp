#include "display_window_builder.h"

#include "Signboard/Platform/primitive/display_window.h"
#include "Signboard/Platform/core/context.h"

#include <stdexcept>

namespace platform::procedure {

	display_window_builder::display_window_builder(const platform::core::context& ctx) 
	{

	}

	display_window_builder& display_window_builder::set_windowTitle(std::string& title) {
		m_title = &title;
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

	platform::primitive::display_window display_window_builder::build() {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		GLFWmonitor* moniter = nullptr;

		if (m_fullscreen) {
			moniter = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(moniter);

			m_extent.width = mode->width;
			m_extent.height = mode->height;
		}
			
		GLFWwindow* glfw_window = glfwCreateWindow(m_extent.width, m_extent.height, (*m_title).c_str(), moniter, nullptr);

		if (!glfw_window)
			throw std::runtime_error("failed to create display window!");

		platform::primitive::display_window l_window;
		l_window.m_window = glfw_window;
		
		return l_window;
	}

}