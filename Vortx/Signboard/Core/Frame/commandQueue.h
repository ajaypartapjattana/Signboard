#pragma once

#include <vector>
#include <string>

enum class CommandType {
	Load,
	Save,
	Action
};

enum class ResourceType {
	Mesh,
	Texture,
	Config
};

struct FrameCommand {
	CommandType type;
	union {
		struct {
			std::string path;
			ResourceType type;
		} resource;
		struct {

		} action;
	};
};