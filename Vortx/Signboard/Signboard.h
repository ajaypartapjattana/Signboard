#pragma once

#include "Core/Interfaces/appInterfaceTypes.h"

#include "Platform/window/windowSurface.h"

#include "RHI/VulkanRHI.h"
#include "resources/ResourceAPI.h"
#include "renderer/Renderer.h"

struct GLFWwindow;

class Signboard {
public:
	Signboard();

	void run();

private:
	StartupConfig loadBaseAppConfiguration();

private:
	StartupConfig baseConfig;

	WindowSurface window;
	VulkanRHI vulkanRHI;
	
	ResourceAPI resources;

	Renderer renderer;
};