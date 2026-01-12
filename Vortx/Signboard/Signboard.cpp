#include "Signboard.h"

#include "Assets/configLoader/ConfigLoader.h"

#include <GLFW/glfw3.h>

Signboard::Signboard()
	: baseConfig(loadBaseAppConfiguration()),
	  window(baseConfig.windowCreateInfo),

	  vulkanRHI(window.getWindowHandle()),
	  resources(vulkanRHI.getDevice()),
	  renderer(vulkanRHI.getRHIView(), resources.getResourceView(), resources.getSceneView())
{

}

void Signboard::run() {

}

StartupConfig Signboard::loadBaseAppConfiguration() {
	ConfigLoader loader;

	StartupConfig config;
	loader.Build_StartupConfig("", config);
	return config;
}