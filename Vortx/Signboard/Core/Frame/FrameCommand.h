#pragma once

enum class ResourceType {
	Model,
	Texture,
	Config
};

enum class ResourceAction {
	LOAD,
	SAVE
};

#include <string>

struct ResourceCommand {
	ResourceType type;
	ResourceAction action;
	std::string path;
};

enum class ACTION {
	MOVE_forward,
	MOVE_Right,
	MOVE_Up,
	Interact,
	Delete,

	Escape,
	Enter
};

struct ActionCommand {
	ACTION action;
	double delta;
};

#include <variant>

using FrameCommandPayload = std::variant<ResourceCommand, ActionCommand>;

struct FrameCommand {
	FrameCommandPayload payload;
};