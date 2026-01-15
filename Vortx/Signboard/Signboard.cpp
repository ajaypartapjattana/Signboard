#include "Signboard.h"

#include "Assets/configLoader/ConfigLoader.h"

Signboard::Signboard()
	: baseConfig(loadBaseAppConfiguration()),
	  window(baseConfig.windowCreateInfo),
	  appCommands(window.getWindowHandle(), ),

	  vulkanRHI(window.getWindowHandle()),
	  resources(vulkanRHI.getDevice(), vulkanRHI.getDescriptorPool()),
	  scene(vulkanRHI.getDevice()),
	  renderer(vulkanRHI.getRHIView(), resources.getResourceView(), scene.getSceneView())
{

}

StartupConfig Signboard::loadBaseAppConfiguration() {
	ConfigLoader loader;

	StartupConfig config;
	loader.Build_StartupConfig("", config);
	return config;
}

void Signboard::run() {
	appCommands.dispatch(EventQueue);
	for (const FrameCommand& command : EventQueue)
		handleCommand(command);

	if (renderer.prepareFrame()) {
		renderer.renderFrame();
	}
}

#include "Core/Frame/FrameCommand.h"

void Signboard::handleCommand(const FrameCommand& cmd) {
	std::visit([](auto&& payload) {
		using T = std::decay_t<decltype(payload)>;

		if constexpr (std::is_same_v<T, ResourceCommand>) {

		}
		else if constexpr (std::is_same_v<T, ActionCommand>) {

		}
	}, cmd.payload);
}