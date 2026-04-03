#pragma once

#include <array>
#include <vector>
#include <memory>

#include "Context/render_context.h"
#include "RenderBackend/rndr_Backend.h"

constexpr uint32_t IMAGE_COUNT = 2;

class Renderer {
public:
	Renderer(const RHIContext& context, const Resources& resources, const Scene& scene);

	~Renderer() noexcept;
		
	void render();
	void configurePresentation(uint32_t* imageCount);

private:
	bool prepareFrame();
	void renderFrame();
	void endFrame();

private:
	resourceView r_resourceRead;

	rndr_presentation m_presentation;
	rndr_method m_methods;
	rndr_interface m_interface;
	rndr_transfer m_transfer;
	rndr_framedraw m_framedraw;

	uint32_t acquiredImage = 0;

};