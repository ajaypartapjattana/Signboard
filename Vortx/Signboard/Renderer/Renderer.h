#pragma once

#include <array>
#include <vector>
#include <memory>

#include "RenderBackend/rndr_Backend.h"
#include "Materials/materials.h"
#include "Pass/passes.h"

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
	rndr_context m_context;
	rndr_interface m_interface;

	passes m_passes;
	materials m_materials;

};