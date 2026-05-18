#pragma once

#include "Signboard/Platform/platform.h"

namespace rndr {
	
	struct _SGBpAccess;

	class creWindow {
	public:
		creWindow();

		bool isAvailable() const noexcept;
		void waitForEvents() const noexcept;

		const plf::displayWindow& native_target() const noexcept;
		plf::windowEventState& get_eventState() noexcept;

	private:
		plf::displayWindow::createInfo describe_window();

	private:
		friend struct _pAccess;

		plf::context m_ctx;

		plf::displayWindow m_primaryWindow;
		plf::windowEventState m_eventState;

	};

}

