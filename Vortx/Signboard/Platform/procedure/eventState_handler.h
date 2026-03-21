#pragma once

namespace platform::primitive {
	class displayWindow;
}

struct InputEvent;

#include <GLFW/glfw3.h>

namespace platform::procedure {

	class displayWindowHandler {
	public:
		displayWindowHandler(platform::primitive::displayWindow& displayWindow);

		bool isAlive() const noexcept;
		
		void waitEvents() const noexcept;
		void poll() const noexcept;

	private:
		GLFWwindow* r_window;

	};

}