#include "Signboard/Platform/internal/plf_pAccess.h"

namespace plf {

	windowEventState::windowEventState(const displayWindow& window) noexcept {
		attachWindow(window);
	}

	void windowEventState::attachWindow(const displayWindow& window) noexcept {
		pWindow = _pAccess::extract(window);

		glfwSetWindowUserPointer(pWindow, this);

		glfwSetFramebufferSizeCallback(pWindow, glfw_callbacks::framebufferResizeCallback);
		glfwSetDropCallback(pWindow, glfw_callbacks::fileDropCallback);
		glfwSetKeyCallback(pWindow, glfw_callbacks::keyCallback);
		glfwSetScrollCallback(pWindow, glfw_callbacks::scrollCallback);
		glfwSetMouseButtonCallback(pWindow, glfw_callbacks::mouseButtonCallback);
		glfwSetCursorPosCallback(pWindow, glfw_callbacks::cursorMoveCallback);
	}

	bool windowEventState::isWindowAlive() const noexcept {
		return !glfwWindowShouldClose(pWindow);
	}

	void windowEventState::waitWindowEvents() const noexcept {
		glfwWaitEvents();
	}

	void windowEventState::pollWindowEvents() const noexcept {
		glfwPollEvents();
	}

	sgb::span<const InputEvent> windowEventState::input_events() const noexcept {
		return sgb::span<const InputEvent>{ m_inputEvents };
	}

	sgb::span<const std::string> windowEventState::fileDrop_events() const noexcept {
		return sgb::span<const std::string>{ m_fileDrops };
	}

	uint64_t windowEventState::windowResize_event() const noexcept {
		return windowResize;
	}

	void windowEventState::windowResize_markClean() noexcept {
		windowResize = event::markClean(windowResize);
	}

}