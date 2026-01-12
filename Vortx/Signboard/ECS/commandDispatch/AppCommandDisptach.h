#pragma once

#include "Signboard/Platform/EventHandler/EventPoll.h"
#include "InputResolve/InputResolver.h"

#include "Signboard/Core/Interfaces/KeyMappingConfigType.h"

struct GLFWwindow;

class CommandDispatcher {
public:
	CommandDispatcher(GLFWwindow* window, const InputMapping& mapping);

	void dispatch();

private:
	bool istriggered(const InputBinding&) const;

private:
	EventPoller platformEvents;

	InputResolver m_resolver;
	InputMapping m_mapping;

};