#include "target_surface.h"

target_surface::target_surface() 
	: 
	m_primary(setup_primiary()),

	m_handler(m_eventState)
{
	platform::procedure::eventState_initializer l_stateInitializer{ m_primary };

	l_stateInitializer.attach(m_eventState);
	l_stateInitializer.initiate_callbacks();
}

bool target_surface::target_ready() {
	m_handler.poll();

	return m_handler.target_isAlive();
}

const platform::primitive::display_window& target_surface::native_target() const noexcept {
	return m_primary;
}

platform::primitive::display_window target_surface::setup_primiary() {
	platform::procedure::display_window_builder l_builder{ m_ctx };

	platform::procedure::display_window_builder::window_extent extent = { 1200, 800 };
	l_builder.setMode_windowed(extent);

	std::string title = "My window";
	l_builder.set_windowTitle(title);

	return l_builder.build();
}