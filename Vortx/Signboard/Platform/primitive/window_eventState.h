#pragma once

struct InputEvent {

	enum class Type {
		Key,
		MouseButton,
		CursorMove,
		Scroll,
		FileDrop
	} type;

	float timestamp;

	union {
		struct {
			int key;
			int scancode;
			int action;
			int mods;
		} key;

		struct {
			int button;
			int action;
			int mods;
		} mouseButton;

		struct {
			double x;
			double y;
		} cursorMove;

		struct {
			double xOffset;
			double yOffset;
		} scroll;

		struct {
			int count;
			const char** paths;
		} fileDrop;
	};

};

namespace platform::procedure {
	class eventState_initializer;
}

#include <GLFW/glfw3.h>
#include <vector>
#include <functional>

namespace platform::primitive {

	struct window_eventState_pAccess;

	class window_eventState {
	public:
		window_eventState();

		window_eventState(const window_eventState&) = delete;
		window_eventState& operator=(const window_eventState&) = delete;

		bool window_isAlive() const;

	private:
		std::function<void(platform::primitive::window_eventState&, int width, int height)> s_onFramebufferResize;
		std::function<void(platform::primitive::window_eventState&, int count, const char** paths)> s_onFileDrop;

		std::function<void(platform::primitive::window_eventState&, int, int, int, int)> s_keyCallback;
		std::function<void(platform::primitive::window_eventState&, double, double)> s_scrollCallback;
		std::function<void(platform::primitive::window_eventState&, int, int, int)> s_mouseButtonCallback;
		std::function<void(platform::primitive::window_eventState&, double, double)> s_cursorMoveCallback;

	private:
		friend class platform::procedure::eventState_initializer;
		friend struct window_eventState_pAccess;

		GLFWwindow* m_window = nullptr;

		std::vector<InputEvent> m_resolveList;

	};

}