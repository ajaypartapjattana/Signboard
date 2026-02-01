#pragma once

#include "display_window.h"

namespace platform::primitive {

	struct display_window_glfwAccess {

		static GLFWwindow* get(const display_window& w) {
			return w.m_window;
		}

	};

}