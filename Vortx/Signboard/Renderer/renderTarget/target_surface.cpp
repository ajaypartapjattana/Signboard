#include "target_surface.h"

target_surface::target_surface() 
	: 
	m_primaryWindow(get_windowCreateInfo()),
	m_handler(m_primaryWindow)
{
	platform::procedure::eventState_initializer l_stateInitializer{ m_primaryWindow };

	l_stateInitializer.attach(m_eventState);
	l_stateInitializer.initiate_callbacks();
}

platform::primitive::displayWindow::createInfo target_surface::get_windowCreateInfo() {
	platform::primitive::displayWindow::createInfo info{};

	info.title = "Signboard";
	info.fullscreen = false;
	info.extent = { 1200, 800 };

	return info;
}

bool target_surface::isAvailable() const noexcept {
	m_handler.poll();
	return m_handler.isAlive();
}

void target_surface::waitForEvents() const noexcept {
	m_handler.waitEvents();
}

const platform::primitive::displayWindow& target_surface::native_target() const noexcept {
	return m_primaryWindow;
}

platform::primitive::windowEventState& target_surface::get_eventState() noexcept {
	return m_eventState;
}
