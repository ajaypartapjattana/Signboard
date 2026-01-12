#include "AppCommandDisptach.h"

#include <GLFW/glfw3.h>
#include <iostream>

CommandDispatcher::CommandDispatcher(GLFWwindow* window, const InputMapping& mapping) 
	: platformEvents(window), m_mapping(mapping)
{

}

void CommandDispatcher::dispatch() {
	platformEvents.poll();

	auto& events = m_resolver.getFrameEvents();
	platformEvents.fetchEvents(events);

	m_resolver.resolveInputs();

	if (m_resolver.isKeyHeld(GLFW_KEY_W))
		std::cout << "keypressed : W" << std::endl;

	for (const InputBinding& binding : m_mapping) {
		if(istriggered(binding)) {
			appFrameQueue.push_back(binding.command);
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