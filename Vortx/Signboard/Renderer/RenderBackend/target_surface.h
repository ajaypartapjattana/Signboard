#pragma once

#include "Signboard/Platform/platform.h"

class target_surface {
public:
	target_surface();

	const platform::primitive::display_window& native_target() const noexcept;

private:
	platform::primitive::display_window setup_primiary();

private:
	platform::core::context m_ctx;

	platform::primitive::display_window m_primary;

};