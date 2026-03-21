#include "eventState_handler.h"

#include "Signboard/Platform/primitive/display_window_glfwAccess.h"

namespace platform::procedure {

	displayWindowHandler::displayWindowHandler(platform::primitive::displayWindow& displayWindow)
		: 
		r_window(platform::primitive::displayWindow_pAccess::get(displayWindow))
	{

	}

	bool displayWindowHandler::isAlive() const noexcept {
		return !glfwWindowShouldClose(r_window);
	}

	void displayWindowHandler::waitEvents() const noexcept {
		glfwWaitEvents();
	}

	void displayWindowHandler::poll() const noexcept {
		glfwPollEvents();
	}

}