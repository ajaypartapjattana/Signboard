#include "Signboard.h"

Signboard::Signboard()
	: 
	m_renderer(m_target.native_target()),
	targetVisible(true),

	m_dispatcher(bindings, toExecuteCommands, targetVisible)
{
	populateCommandTable();
}

void Signboard::populateCommandTable() {
	commandTable[(uint32_t)CommandID::CONFIG] = &Signboard::routine_Config;
}

void Signboard::run() {
	while (m_target.isAvailable()) {
		m_dispatcher.resolveSurfaceEvents(m_target.get_eventState());

		if (!targetVisible) {
			m_target.waitForEvents();
			continue;
		}

		m_dispatcher.resolveInputEvents(m_target.get_eventState());

		if (!executeCommands())
			continue;

		m_renderer.render();

	}
}

bool Signboard::executeCommands() {
	for (FrameCommand& cmd : toExecuteCommands) {
		auto fn = commandTable[(uint32_t)cmd.id];
		if (fn && fn(*this, cmd.data)) {
			toExecuteCommands.clear();
			return false;
		}
	}

	toExecuteCommands.clear();
	return true;
}

bool Signboard::routine_Config(Signboard& board, glm::vec2 data) {
	board.m_renderer.configurePresentation();
	return true;
}
	