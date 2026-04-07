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
	void setupDefaults();

private:
	using CommandFn = bool(*)(Signboard&, glm::vec2);
	std::array<CommandFn, MAX_COMMAND_COUNT> commandTable;

	void populateCommandTable();
	bool executeCommands();

	bool targetVisible;

	static bool routine_Config(Signboard&, glm::vec2);
	static bool routine_upload(Signboard&, glm::vec2);

private:
	target_surface m_target;

	RHIContext m_context;
	
	Resources m_resources;
	Scene m_scene;

	Mesher m_mesher;
	Renderer m_renderer;

	InputMapping bindings = { {GLFW_KEY_W, InputTrigger::Pressed, CommandID::CONFIG} };
	std::vector<FrameCommand> toExecuteCommands;
	CommandDispatcher m_dispatcher;

};