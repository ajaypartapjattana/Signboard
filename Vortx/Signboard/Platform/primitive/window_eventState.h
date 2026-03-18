#pragma once

namespace platform::procedure {
	class eventState_initializer;
	class eventState_handler;
}

namespace platform::detail {
	struct glfw_callbacks;
}

#include <GLFW/glfw3.h>
#include <vector>
#include <functional>

#include "Signboard/Platform/detail/InputEvent.h"

namespace platform::primitive {

	class window_eventState {
	public:
		window_eventState();

		window_eventState(const window_eventState&) = delete;
		window_eventState& operator=(const window_eventState&) = delete;

		static window_eventState* from(GLFWwindow* window);

		bool window_isAlive() const;

	private:
		friend class platform::procedure::eventState_initializer;
		friend class platform::procedure::eventState_handler;

		friend struct detail::glfw_callbacks;

		GLFWwindow* m_window = nullptr;

		std::vector<platform::detail::InputEvent> m_resolveList;

	};

}