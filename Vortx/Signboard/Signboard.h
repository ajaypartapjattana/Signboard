#pragma once

#include "Renderer/renderTarget/target_surface.h"
#include "Input/AppCommandDisptach.h"
#include "Renderer/Renderer.h"
#include "Resources/resources.h"
#include "Scene/Scene.h"

constexpr uint32_t MAX_COMMAND_COUNT = 32;

class Signboard {
public:
	Signboard();

	void run();

private:
	using CommandFn = bool(*)(Signboard&, glm::vec2);
	std::array<CommandFn, MAX_COMMAND_COUNT> commandTable;

	void populateCommandTable();
	bool executeCommands();

	bool targetVisible;

	static bool routine_Config(Signboard&, glm::vec2);

private:
	target_surface m_target;

	RHIContext m_context;
	Renderer m_renderer;
	Resources m_resources;
	Scene m_scene;

	InputMapping bindings = { {GLFW_KEY_W, InputTrigger::Pressed, CommandID::CONFIG} };
	std::vector<FrameCommand> toExecuteCommands;
	CommandDispatcher m_dispatcher;

};