#include "eventState_handler.h"

#include "Signboard/Platform/primitive/window_eventState.h"

namespace platform::procedure {

	eventState_handler::eventState_handler(platform::primitive::window_eventState& state)
		: m_eventState(state)
	{

	}

	void eventState_handler::poll() {
		m_eventState.m_resolveList.clear();
		glfwPollEvents();
	}

	void eventState_handler::fetch_eventList(std::vector<platform::detail::InputEvent>& target_list) {
		target_list.clear();
		target_list.swap(m_eventState.m_resolveList);
	}

	bool eventState_handler::target_isAlive() {
		return !glfwWindowShouldClose(m_eventState.m_window);
	}

}