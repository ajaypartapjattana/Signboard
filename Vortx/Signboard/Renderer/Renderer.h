#pragma once

#include <array>
#include <vector>
#include <memory>

#include "RenderBackend/rndr_Backend.h"

constexpr uint32_t IMAGE_COUNT = 2;

class Renderer {
public:
	Renderer(const platform::primitive::displayWindow& render_target);

	~Renderer() noexcept;

	void render();

	void configurePresentation(uint32_t* imageCount);

private:
	bool prepareFrame();
	void renderFrame();
	void endFrame();

private:
	rndr_context m_context;
	rndr_presentation m_presentation;
	rndr_method m_methods;
	rndr_interface m_interface;
	rndr_framedraw m_framedraw;

	uint32_t acquiredImage = 0;

};