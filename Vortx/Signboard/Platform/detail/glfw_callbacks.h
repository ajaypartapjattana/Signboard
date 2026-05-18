#pragma once

#include "Signboard/Platform/internal/plf_pAccess.h"

namespace plf {

	static windowEventState* from(GLFWwindow* window) {
		return static_cast<windowEventState*>(glfwGetWindowUserPointer(window));
	}

	struct glfw_callbacks {

		static void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
			if (auto* state = from(window)) {
				state->m_inputEvents.push_back( InputEvent{ event::encodeKey(key, scanCode, action, mods) } );
			}
		}

		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
			if (auto* state = from(window)) {
				state->m_inputEvents.push_back( InputEvent{ event::encodeMouse(button, action, mods) } );
			}
		}

		static void cursorMoveCallback(GLFWwindow* window, double xPos, double yPos) {
			if (auto* state = from(window)) {
				state->m_inputEvents.push_back( InputEvent{ event::encodeCursor(static_cast<float>(xPos), static_cast<float>(yPos)) } );
			}
		}

		static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
			if (auto* state = from(window)) {
				state->m_inputEvents.push_back( InputEvent{ event::encodeScroll(static_cast<float>(xOffset), static_cast<float>(yOffset)) } );
			}
		}

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
			if (auto* state = from(window)) {
				state->windowResize = event::encodeResize(true, width, height);
			}
		}

		static void fileDropCallback(GLFWwindow* window, int count, const char** paths) {
			if (auto* state = from(window)) {
				state->m_fileDrops.clear();
				state->m_fileDrops.reserve(count);
				for(int i = 0; i < count; ++i)
					state->m_fileDrops[i] = paths[i];
			}
		}

	};

}