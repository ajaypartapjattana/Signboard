#pragma once

#include "Core/Interfaces/appInterfaceTypes.h"

#include "Platform/window/windowSurface.h"

#include "ECS/commandDispatch/AppCommandDisptach.h"

#include "RHI/VulkanRHI.h"
#include "resources/Resource.h"
#include "renderer/Renderer.h"

struct GLFWwindow;
struct FrameCommand;

class Signboard {
public:
	Signboard();

	void run();

private:
	StartupConfig loadBaseAppConfiguration();

	void handleCommand(const FrameCommand& cmd);

private:
	StartupConfig baseConfig;

	WindowSurface window;
	CommandDispatcher appCommands;

	VulkanRHI vulkanRHI;
	
	ResourceAPI resources;
	Scene scene;

	Renderer renderer;

	std::vector<FrameCommand> EventQueue;
};