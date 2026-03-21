#pragma once

#include "Signboard/Platform/platform.h"

class target_surface {
public:
	target_surface();

	bool isAvailable() const noexcept;
	void waitForEvents() const noexcept;

	const platform::primitive::displayWindow& native_target() const noexcept;
	platform::primitive::windowEventState& get_eventState() noexcept;

private:
	platform::primitive::displayWindow::createInfo get_windowCreateInfo();

private:
	platform::core::context m_ctx;

	platform::primitive::displayWindow m_primaryWindow;
	platform::procedure::displayWindowHandler m_handler;

	platform::primitive::windowEventState m_eventState;

};