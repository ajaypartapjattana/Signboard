#pragma once

#include <array>
#include <vector>
#include <memory>

#include "RenderBackend/render_interface.h"

class Renderer {
public:
	Renderer(const platform::primitive::display_window& render_target);

	void render();

private:

	bool prepareFrame();
	void renderFrame();
	void endFrame();

	void resize();

private:
	render_interface m_renderInterface;

};