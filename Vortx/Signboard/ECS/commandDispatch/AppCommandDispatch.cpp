#include "AppCommandDisptach.h"

#include <GLFW/glfw3.h>
#include <iostream>

#include "Signboard/Platform/primitive/display_window_glfwAccess.h"

CommandDispatcher::CommandDispatcher(const platform::primitive::display_window& window, const InputMapping& mapping) 
	: platformEvents(platform::primitive::display_window_glfwAccess::get(window)), m_mapping(mapping)
{

}

#include "Signboard/Core/Frame/FrameCommand.h"

void CommandDispatcher::dispatch(std::vector<FrameCommand>& appEventQueue) {
	platformEvents.poll();

	auto& events = m_resolver.getFrameEvents();
	platformEvents.fetchEvents(events);

	m_resolver.resolveInputs();

	if (m_resolver.isKeyHeld(GLFW_KEY_W))
		std::cout << "keypressed : W" << std::endl;

	for (const InputBinding& binding : m_mapping) {
		if(istriggered(binding)) {
			FrameCommand cmd;
			cmd.payload = ResourceCommand{ResourceType::Model, ResourceAction::LOAD, "assets_data/models/Femhand.obj"};

			appEventQueue.push_back(cmd);
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