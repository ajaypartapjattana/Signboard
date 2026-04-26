#pragma once

#include "Signboard/Core/Interfaces/renderer/rendererINF.h"

#include "Context/render_context.h"
#include "Signboard/Resources/resources_pAccess.h"
#include "RenderBackend/rndr_Backend.h"

constexpr uint32_t IMAGE_COUNT = 2;

class Renderer {
public:
	Renderer(const RHIContext& context, Resources& resources, const Scene& scene);

	~Renderer() noexcept;
		
	void render();
	void configurePresentation(uint32_t* imageCount);

	void queueUpload(const Model& model, uint32_t allocatedMesh);

private:
	bool defferUploads();
	bool prepareFrame();

private:
	ResourceView r_resources;

	rndr_presentation m_presentation;
	rndr_method m_methods;
	rndr_interface m_interface;
	rndr_transfer m_transfer;
	rndr_framedraw m_framedraw;

	uint32_t acquiredImage = 0;

};