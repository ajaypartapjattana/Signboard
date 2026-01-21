#pragma once

#include <GLFW/glfw3.h>
#include "display_window.h"

namespace window::primitive {

	struct display_window_glfwAccess {

		static GLFWwindow* get(const display_window& w) noexcept {
			return reinterpret_cast<GLFWwindow*>(w.m_window);
		}

	};

}