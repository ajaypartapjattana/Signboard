#include "target_surface.h"

target_surface::target_surface() 
	: m_primary(setup_primiary())
{

}

const platform::primitive::display_window& target_surface::native_target() const noexcept {
	return m_primary;
}

platform::primitive::display_window target_surface::setup_primiary() {
	platform::procedure::display_window_builder l_builder{ m_ctx };

	platform::procedure::display_window_builder::window_extent extent = { 800, 600 };
	l_builder.setMode_windowed(extent);

	std::string title = "My window";
	l_builder.set_windowTitle(title);

	return l_builder.build();
}