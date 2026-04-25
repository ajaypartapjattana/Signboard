#include "renderer.h"

Renderer::Renderer(const RHIContext& context, Resources& resources, const Scene& scene)
	: 
	r_resources(resources.read_resources()),

	m_presentation(context, resources, IMAGE_COUNT),
	m_methods(context, m_presentation),
	m_interface(context, m_presentation),
	m_transfer(context, r_resources),
	m_framedraw(m_methods, r_resources)
{

}

Renderer::~Renderer() noexcept {

}

void Renderer::render() {
	bool waitUploads = defferUploads();

	if (!prepareFrame()) {
		m_interface.advance_frame();
		return;
	}

	m_framedraw.drawFrame(acquiredImage, m_interface.get_graphicsCMD());
	m_interface.submit_graphics(waitUploads);

	m_interface.present_activeFrame();
	m_interface.advance_frame();
}

bool Renderer::prepareFrame() {
	VkBool32 frameReady = VK_TRUE;
	acquiredImage = m_interface.acquire_toWriteImage(&frameReady);

	if (frameReady)	
		return true;

	configurePresentation(nullptr);
	return false;
}

bool Renderer::defferUploads() {
	VkResult result = m_transfer.recordUploads(m_interface.get_transferCMD());

	if (result == VK_INCOMPLETE)
		return false;

	m_interface.submit_tranfer_cmd();
	m_transfer.reset();

	return true;
}

void Renderer::configurePresentation(uint32_t* imageCount) {
	m_presentation.recreate_swapchain(imageCount);
	m_methods.validate_primaryTarget();
	m_interface.validate_swapchainDependancy();

	if (imageCount) {
		m_interface.configure_bufferedFrames();
	}
}

void Renderer::queueUpload(const Model& model, uint32_t allocatedMesh) {
	const Mesh& _mesh = *r_resources.meshes.get(allocatedMesh);

	m_transfer.stageUpload(UploadSpan(model.vertices), { _mesh.vertexBuffer, 0 });
	m_transfer.stageUpload(UploadSpan(model.indices), { _mesh.indexBuffer, 0 });
}
