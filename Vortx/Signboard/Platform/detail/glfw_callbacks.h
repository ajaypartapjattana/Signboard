#pragma once

#include "Signboard/Platform/primitive/window_eventState.h"

namespace platform::detail {

	struct glfw_callbacks {

		static void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
			if (auto* state = platform::primitive::windowEventState::from(window)) {
				state->m_inputEvents.push_back( InputEvent{ frame::event::encodeKey(key, scanCode, action, mods) } );
			}
		}

		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
			if (auto* state = platform::primitive::windowEventState::from(window)) {
				state->m_inputEvents.push_back( InputEvent{ frame::event::encodeMouse(button, action, mods) } );
			}
		}

		static void cursorMoveCallback(GLFWwindow* window, double xPos, double yPos) {
			if (auto* state = platform::primitive::windowEventState::from(window)) {
				state->m_inputEvents.push_back( InputEvent{ frame::event::encodeCursor(static_cast<float>(xPos), static_cast<float>(yPos)) } );
			}
		}

		static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
			if (auto* state = platform::primitive::windowEventState::from(window)) {
				state->m_inputEvents.push_back( InputEvent{ frame::event::encodeScroll(static_cast<float>(xOffset), static_cast<float>(yOffset)) } );
			}
		}

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
			if (auto* state = platform::primitive::windowEventState::from(window)) {
				state->windowResize = frame::event::encodeResize(true, width, height);
			}
		}

		static void fileDropCallback(GLFWwindow* window, int count, const char** paths) {
			if (auto* state = platform::primitive::windowEventState::from(window)) {
				state->m_fileDrops.reserve(count);
				for(int i = 0; i < count; ++i)
					state->m_fileDrops.push_back(paths[i]);
			}
		}

	};

}