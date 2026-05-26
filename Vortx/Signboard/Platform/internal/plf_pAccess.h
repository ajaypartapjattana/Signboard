#pragma once

#include "Signboard/Platform/platform.h"

namespace plf {

	struct _pAccess {

		static GLFWwindow* extract(const window& w) {
			return w.m_window;
		}

	};

}