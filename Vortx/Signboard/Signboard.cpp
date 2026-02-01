#include "Signboard.h"

#include "Assets/configLoader/ConfigLoader.h"

Signboard::Signboard()
	: m_renderer(m_target.native_target())
{

}

void Signboard::run() {
	while (m_target.target_ready()) {

	}
}

#include "Core/Frame/FrameCommand.h"

void Signboard::handleCommand(const FrameCommand& cmd) {
	std::visit([](auto&& payload) {
		using T = std::decay_t<decltype(payload)>;

		if constexpr (std::is_same_v<T, ResourceCommand>) {

		}
		else if constexpr (std::is_same_v<T, ActionCommand>) {

		}
	}, cmd.payload);
}