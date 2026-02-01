#pragma once

#include "Signboard/Platform/EventHandler/EventPoll.h"
#include "InputResolve/InputResolver.h"

#include "Signboard/Core/Interfaces/KeyMappingConfigType.h"

struct GLFWwindow;
struct FrameCommand;

namespace platform::primitive {
	class display_window;
}

class CommandDispatcher {
public:
	CommandDispatcher(const platform::primitive::display_window& window, const InputMapping& mapping);

	void dispatch(std::vector<FrameCommand>& appEventQueue);

private:
	bool istriggered(const InputBinding&) const;

private:
	EventPoller platformEvents;

	InputResolver m_resolver;
	InputMapping m_mapping;

};