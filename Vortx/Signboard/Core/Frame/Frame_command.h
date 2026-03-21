#pragma once

#include <cstdint>
#include <GLFW/glfw3.h>

enum class CommandID : uint32_t {
	CONFIG,
	ESCAPE
};

struct FrameCommand {
	CommandID id;
	glm::vec2 data;
};

inline FrameCommand makeCommand(CommandID id, glm::vec2 data = glm::vec2{ 0, 0 }) {
	return { id, data };
}
