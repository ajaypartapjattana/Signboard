#include "target_surface.h"

target_surface::target_surface() 
	: m_primary(setup_primiary())
{
	platform::procedure::eventState_initializer l_stateInitializer{ m_eventState };

	l_stateInitializer.attach(m_primary);
}

bool target_surface::target_ready() {
	platform::procedure::window_eventHandler l_handler{ m_eventState };
	l_handler.poll();
	return l_handler.target_isAlive();
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