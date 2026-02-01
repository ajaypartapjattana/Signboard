#pragma once

#include "window_eventState.h"

namespace platform::primitive {

	struct window_eventState_pAccess {

		static GLFWwindow* get_window(const window_eventState& es) {
			return es.m_window;
		}

		static const std::function<void(platform::primitive::window_eventState&, int, int)>& getf_framebufferResize(const window_eventState& es) {
			return es.s_onFramebufferResize;
		}

		static const std::function<void(platform::primitive::window_eventState&, int, const char**)>& getf_fileDrop(const window_eventState& es) {
			return es.s_onFileDrop;
		}

		static const std::function<void(platform::primitive::window_eventState&, int, int, int, int)>& getf_keyCallback(const window_eventState& es) {
			return es.s_keyCallback;
		}

		static const std::function<void(platform::primitive::window_eventState&, double, double)>& getf_scrollCallback(const window_eventState& es) {
			return es.s_scrollCallback;
		}

		static const std::function<void(platform::primitive::window_eventState&, int, int, int)>& getf_mouseButtonCallback(const window_eventState& es) {
			return es.s_mouseButtonCallback;
		}

		static const std::function<void(platform::primitive::window_eventState&, double, double)>& getf_cursorMoveCallback(const window_eventState& es) {
			return es.s_cursorMoveCallback;
		}

		static std::vector<InputEvent>& get_eventList(window_eventState& es) noexcept {
			return es.m_resolveList;
		}

	};

}