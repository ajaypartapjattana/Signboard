#include "renderer.h"

Renderer::Renderer(const RHIContext& context, const Resources& resources, const Scene& scene)
	: 
	r_resourceRead(resources.read_resources()),

	m_presentation(context, IMAGE_COUNT),
	m_methods(context, m_presentation),
	m_interface(context, m_presentation),
	m_transfer(context, r_resourceRead),
	m_framedraw(m_methods, r_resourceRead)
{

}

Renderer::~Renderer() noexcept {

}

void Renderer::render() {
	if (!prepareFrame()) {
		m_interface.advance_frame();
		return;
	}

	renderFrame();
	endFrame();
}

bool Renderer::prepareFrame() {
	VkBool32 frameReady = VK_TRUE;
	acquiredImage = m_interface.acquire_toWriteImage(&frameReady);

	if (frameReady)	
		return true;

	configurePresentation(nullptr);
	return false;
}

void Renderer::renderFrame() {
	m_framedraw.drawFrame(acquiredImage, m_interface.get_activeFrame_cmd());
	m_interface.submit_activeFrame_cmd();
}

void Renderer::endFrame() {
	m_interface.present_activeFrame();
	m_interface.advance_frame();
}

void Renderer::configurePresentation(uint32_t* imageCount) {
	m_presentation.recreate_swapchain(imageCount);
	m_methods.validate_primaryTarget();
	m_interface.validate_swapchainDependancy();

	if (imageCount) {
		m_interface.configure_bufferedFrames();
	}
}
