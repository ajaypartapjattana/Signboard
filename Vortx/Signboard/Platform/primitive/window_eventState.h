#pragma once

namespace platform::procedure {
	class eventState_initializer;
	class displayWindowHandler;
}

namespace platform::detail {
	struct glfw_callbacks;
}

#include <GLFW/glfw3.h>
#include <vector>
#include <functional>

#include "Signboard/Core/Frame/Frame_event.h"

namespace platform::primitive {

	class eventStateInputsAccess;
	class eventStateSurfaceAccess;

	class windowEventState {
	public:
		windowEventState();

		windowEventState(const windowEventState&) = delete;
		windowEventState& operator=(const windowEventState&) = delete;

		static windowEventState* from(GLFWwindow* window);

	private:
		friend class platform::procedure::eventState_initializer;
		friend class platform::procedure::displayWindowHandler;

		friend struct detail::glfw_callbacks;

		friend class eventStateInputsAccess;
		friend class eventStateSurfaceAccess;

		GLFWwindow* m_window = nullptr;

		std::vector<InputEvent> m_inputEvents;
		std::vector<std::string> m_fileDrops;
		uint64_t windowResize;

	};

	class eventStateInputsAccess {
	public:
		eventStateInputsAccess(windowEventState& eventState) noexcept;

		auto begin() const noexcept { return r_eventState.m_inputEvents.begin(); }
		auto end() const noexcept {	return r_eventState.m_inputEvents.end(); }

		size_t size() const noexcept;
		void drainInputList() noexcept;

	private:
		windowEventState& r_eventState;

	};

	class eventStateSurfaceAccess {
	public:
		eventStateSurfaceAccess(windowEventState& eventState) noexcept;

		const std::vector<std::string>& fileDrops() const noexcept;
		uint64_t windowResizeState() const noexcept;

		void clearFileDrops() noexcept;
		void consumeResize() noexcept;

	private:
		windowEventState& r_eventState;

	};

}