#include "Signboard/Renderer/Internal/rndr_pAccess.h"

//remove this module, make resp for user here, i.e. window initialization and state handling

namespace rndr {

	creWindow::creWindow()
		:
		m_primaryWindow(describe_window()),
		m_eventState(m_primaryWindow)
	{

	}

	plf::window::createInfo creWindow::describe_window() {
		plf::window::createInfo info{};

		info.title = "Signboard";
		info.fullscreen = false;
		info.extent = { 1200, 800 };

		return info;
	}

	bool creWindow::isAvailable() const noexcept {
		m_handler.poll();
		return m_handler.isAlive();
	}

	void creWindow::waitForEvents() const noexcept {
		m_eventState.waitWindowEvents();
	}

	const plf::window& creWindow::native_target() const noexcept {
		return m_primaryWindow;
	}

	plf::windowEventState& creWindow::get_eventState() noexcept {
		return m_eventState;
	}

}
