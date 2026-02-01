#pragma once

#include "renderer/Renderer.h"

struct FrameCommand;

class Signboard {
public:
	Signboard();

	void run();

private:
	void handleCommand(const FrameCommand& cmd);

private:
	target_surface m_target{};
	Renderer m_renderer;

};