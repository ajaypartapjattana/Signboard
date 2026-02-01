#pragma once

#include <GLFW/glfw3.h>

namespace platform::core {

	class window_eventState {
	public:
		void* user = nullptr;

		void (*onFramebufferResize)(void* user, int width, int height) = nullptr;
		void (*onFileDrop)(void* user, int count, const char** path) = nullptr;

	};

}