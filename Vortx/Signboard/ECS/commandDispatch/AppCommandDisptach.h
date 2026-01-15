#pragma once

#include "Signboard/Platform/EventHandler/EventPoll.h"
#include "InputResolve/InputResolver.h"

#include "Signboard/Core/Interfaces/KeyMappingConfigType.h"

struct GLFWwindow;
struct FrameCommand;

class CommandDispatcher {
public:
	CommandDispatcher(GLFWwindow* window, const InputMapping& mapping);

	void dispatch(std::vector<FrameCommand>& appEventQueue);

private:
	bool istriggered(const InputBinding&) const;

private:
	EventPoller platformEvents;

	InputResolver m_resolver;
	InputMapping m_mapping;

};