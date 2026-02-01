#include "window_eventState.h"

#include "Signboard/Platform/primitive/display_window_glfwAccess.h"
#include "window_eventState_pAccess.h"

#include "Signboard/Core/Time/Time.h"
#include <chrono>

namespace {
	static void framebufferResizeCallback(platform::primitive::window_eventState& state, int width, int height) {

	}

	static void fileDropCallback(platform::primitive::window_eventState& state, int count, const char** paths) {
		InputEvent e{};
		e.type = InputEvent::Type::FileDrop;
		e.fileDrop = { count, paths };
		e.timestamp = Time::now();

		platform::primitive::window_eventState_pAccess::get_eventList(state).push_back(e);
	}

	static void keyCallBack(platform::primitive::window_eventState& state, int key, int scanCode, int action, int mods) {
		InputEvent e{};
		e.type = InputEvent::Type::Key;
		e.key = { key, action, mods };
		e.timestamp = Time::now();

		platform::primitive::window_eventState_pAccess::get_eventList(state).push_back(e);
	}

	static void scrollCallBack(platform::primitive::window_eventState& state, double xoffset, double yoffset) {
		InputEvent e{};
		e.type = InputEvent::Type::Scroll;
		e.scroll = { xoffset, yoffset };
		e.timestamp = Time::now();

		platform::primitive::window_eventState_pAccess::get_eventList(state).push_back(e);
	}

	static void mouseButtonCallBack(platform::primitive::window_eventState& state, int button, int action, int mods) {
		InputEvent e{};
		e.type = InputEvent::Type::MouseButton;
		e.mouseButton = { button, action, mods };
		e.timestamp = Time::now();

		platform::primitive::window_eventState_pAccess::get_eventList(state).push_back(e);
	}

	static void cursorMoveCallBack(platform::primitive::window_eventState& state, double xpos, double ypos) {
		InputEvent e{};
		e.type = InputEvent::Type::CursorMove;
		e.cursorMove = { xpos, ypos };
		e.timestamp = Time::now();

		platform::primitive::window_eventState_pAccess::get_eventList(state).push_back(e);
	}
}

namespace platform::primitive {

	window_eventState::window_eventState()
		:
		s_onFramebufferResize(framebufferResizeCallback),
		s_onFileDrop(fileDropCallback),

		s_keyCallback(keyCallBack),
		s_scrollCallback(scrollCallBack),
		s_mouseButtonCallback(mouseButtonCallBack),
		s_cursorMoveCallback(cursorMoveCallBack)
	{

	}

	bool window_eventState::window_isAlive() const {
		return !glfwWindowShouldClose(m_window);
	}

}