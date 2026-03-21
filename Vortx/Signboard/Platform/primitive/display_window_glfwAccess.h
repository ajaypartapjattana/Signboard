#pragma once

#include "display_window.h"

namespace platform::primitive {

	struct displayWindow_pAccess {

		static GLFWwindow* get(const displayWindow& w) {
			return w.m_window;
		}

	};

}