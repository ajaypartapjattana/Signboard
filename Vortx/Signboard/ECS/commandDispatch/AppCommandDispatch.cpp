#include "AppCommandDisptach.h"

#include <GLFW/glfw3.h>
#include <iostream>

CommandDispatcher::CommandDispatcher(const InputMapping& mapping, std::vector<FrameCommand>& appEventQueue, bool& appRoutine)
	:
	m_mapping(mapping),
	targetCommandList(appEventQueue),
	targetVisibility(appRoutine)
{

}

#include "Signboard/Core/Frame/Frame_command.h"

void CommandDispatcher::resolveSurfaceEvents(platform::primitive::windowEventState& eventState) {
	platform::primitive::eventStateSurfaceAccess l_surfaceAccess{ eventState };

	uint64_t a_resizeState = l_surfaceAccess.windowResizeState();
	if (verifyWindowVisibility(a_resizeState))
		l_surfaceAccess.consumeResize();
}

bool CommandDispatcher::verifyWindowVisibility(uint64_t state) noexcept {
	auto data = frame::event::decode_resize(state);

	if (!data.dirty)
		return false;
	
	targetVisibility = (data.width != 0 && data.height != 0);

	if (targetVisibility)
		targetCommandList.push_back(makeCommand(CommandID::CONFIG));

	return true;
}

void CommandDispatcher::resolveInputEvents(platform::primitive::windowEventState& eventState) {
	m_resolver.resolveInputs(platform::primitive::eventStateInputsAccess{ eventState });

	for (const InputBinding& binding : m_mapping) {
		if(istriggered(binding)) {
			targetCommandList.push_back(makeCommand(binding.command));
		}
	}
}

bool CommandDispatcher::istriggered(const InputBinding& binding) const {
	switch (binding.trigger) {
	case InputTrigger::Pressed:		return m_resolver.isKeyPressed(binding.key);
	case InputTrigger::Released:	return m_resolver.isKeyReleased(binding.key);
	case InputTrigger::Held:		return m_resolver.isKeyHeld(binding.key);
	}
	return false;
}