#pragma once

#include <array>
#include <vector>
#include <memory>

#include "RenderBackend/rndr_Backend.h"

class Renderer {
public:
	Renderer(const platform::primitive::display_window& render_target);

	~Renderer() noexcept;

	void render();

private:
	bool prepareFrame();
	void renderFrame();
	void endFrame();

	void configure_presentation();

private:
	rndr_context m_context;
	rndr_presentation m_presentation;
	rndr_method m_methods;
	rndr_interface m_interface;
	rndr_framedraw m_framedraw;

	uint32_t acquiredImage = 0;

};