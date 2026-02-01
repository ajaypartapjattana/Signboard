#include "window_eventHandler.h"

#include "Signboard/Platform/primitive/window_eventState_pAccess.h"

namespace platform::procedure {

	window_eventHandler::window_eventHandler(platform::primitive::window_eventState& state)
		: m_eventState(state)
	{

	}

	void window_eventHandler::poll() {
		platform::primitive::window_eventState_pAccess::get_eventList(m_eventState).clear();
		glfwPollEvents();
	}

	void window_eventHandler::fetch_eventList(std::vector<InputEvent>& target_list) {
		target_list.clear();
		target_list.swap(platform::primitive::window_eventState_pAccess::get_eventList(m_eventState));
	}

	bool window_eventHandler::target_isAlive() {
		return !glfwWindowShouldClose(platform::primitive::window_eventState_pAccess::get_window(m_eventState));
	}

}