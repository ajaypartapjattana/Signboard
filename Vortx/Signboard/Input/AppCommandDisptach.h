#pragma once

#include "InputResolve/InputResolver.h"
#include "Signboard/Core/Interfaces/KeyMappingConfigType.h"

struct GLFWwindow;
struct FrameCommand;

namespace platform::primitive {
	class displayWindow;
}

class CommandDispatcher {
public:
	CommandDispatcher(const InputMapping& mapping, std::vector<FrameCommand>& appEventQueue, bool& targetVisibilty);
	
	void resolveSurfaceEvents(platform::primitive::windowEventState& eventState);
	void resolveInputEvents(platform::primitive::windowEventState& eventState);

private:
	bool verifyWindowVisibility(uint64_t resizeState) noexcept;

	bool istriggered(const InputBinding&) const;

private:
	InputMapping m_mapping;
	std::vector<FrameCommand>& targetCommandList;
	bool& targetVisibility;

	InputResolver m_resolver;

};