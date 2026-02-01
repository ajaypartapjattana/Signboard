#pragma once

#include <array>
#include <vector>
#include <memory>

#include "RenderBackend/render_interface.h"
#include "RenderBackend/target_surface.h"

class Renderer {
public:
	Renderer();

	void render();

private:

	bool prepareFrame();
	void renderFrame();
	void endFrame();

	void resize();

private:
	target_surface m_target;
	render_interface m_renderInterface;

};