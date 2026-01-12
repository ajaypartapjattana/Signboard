#pragma once

#include <vector>
#include <cstdint>

enum class InputTrigger {
	Pressed,
	Released,
	Held
};

using CommandID = uint32_t;

struct InputBinding {
	int key;
	InputTrigger trigger;
	CommandID command;
};

using InputMapping = std::vector<InputBinding>;