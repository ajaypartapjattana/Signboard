#pragma once

#include "Signboard/Platform/platform.h"

namespace plf {

	struct _pAccess {

		static GLFWwindow* extract(const displayWindow& w) {
			return w.m_window;
		}

	};

}