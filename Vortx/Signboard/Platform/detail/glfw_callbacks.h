#pragma once

#include <GLFW/glfw3.h>

#include "Signboard/Platform/primitive/window_eventState.h"

namespace platform::detail {

	struct glfw_callbacks {

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
			if (auto* state = platform::primitive::window_eventState::from(window)) {
				state->m_resolveList.push_back(InputEvent::FramebufferResize(width, height));
			}
		}

		static void fileDropCallback(GLFWwindow* window, int count, const char** paths) {
			if (auto* state = platform::primitive::window_eventState::from(window)) {
				state->m_resolveList.push_back(InputEvent::FileDrop(count, paths));
			}
		}

		static void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
			if (auto* state = platform::primitive::window_eventState::from(window)) {
				state->m_resolveList.push_back(InputEvent::Key(key, scanCode, action, mods));
			}
		}

		static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
			if (auto* state = platform::primitive::window_eventState::from(window)) {
				state->m_resolveList.push_back(InputEvent::Scroll(xOffset, yOffset));
			}
		}

		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
			if (auto* state = platform::primitive::window_eventState::from(window)) {
				state->m_resolveList.push_back(InputEvent::MouseButton(button, action, mods));
			}
		}

		static void cursorMoveCallback(GLFWwindow* window, double xPos, double yPos) {
			if (auto* state = platform::primitive::window_eventState::from(window)) {
				state->m_resolveList.push_back(InputEvent::CursorMove(xPos, yPos));
			}
		}

	};

}