#pragma once

#include <GLFW/glfw3.h>
#include <vector>

#include "Signboard/Core/core.h"

namespace plf {

	class displayWindow;
	class displayWindowHandler;

	struct glfw_callbacks;

	class windowEventState {
	public:
		windowEventState(const displayWindow& window) noexcept;

		windowEventState(const windowEventState&) = delete;
		windowEventState& operator=(const windowEventState&) = delete;

		void attachWindow(const displayWindow& window) noexcept;

		bool isWindowAlive() const noexcept;
		void waitWindowEvents() const noexcept;
		void pollWindowEvents() const noexcept;

		sgb::span<const InputEvent> input_events() const noexcept;
		sgb::span<const std::string> fileDrop_events() const noexcept;
		uint64_t windowResize_event() const noexcept;
		void windowResize_markClean() noexcept;

	private:
		friend class eventState_initializer;
		friend class displayWindowHandler;

		friend struct glfw_callbacks;
		
		std::vector<InputEvent> m_inputEvents;
		std::vector<std::string> m_fileDrops;
		uint64_t windowResize = 0;

		GLFWwindow* pWindow = nullptr;

	};

}