#include "renderer.h"

Renderer::Renderer(const platform::primitive::displayWindow& render_target) 
	: 
	m_context(render_target),
	m_presentation(m_context, 2),
	m_methods(m_context, m_presentation),
	m_interface(m_context, m_presentation),
	m_framedraw(m_methods)
{

}

Renderer::~Renderer() noexcept {

}

void Renderer::render() {
	if (!prepareFrame()) return;

	renderFrame();
	endFrame();
}

bool Renderer::prepareFrame() {
	VkBool32 frameReady = VK_TRUE;
	acquiredImage = m_interface.acquire_toWriteImage(&frameReady);

	if (frameReady)	return true;

	configurePresentation();
	return false;
}

void Renderer::renderFrame() {
	m_framedraw.drawFrame(acquiredImage, m_interface.get_activeFrame_cmd());
	m_interface.submit_activeFrame_cmd();
}

void Renderer::endFrame() {
	m_interface.present_activeFrame(acquiredImage);
	m_interface.advance_frame();
}

void Renderer::configurePresentation() {
	m_presentation.recreate_swapchain();
	m_methods.validate_primaryTarget();
	m_interface.validate_swapchainDependancy();
}
