#include "renderer.h"

Renderer::Renderer(const platform::primitive::display_window& render_target) 
	: 
	m_context(render_target),
	m_presentation(m_context, 2),
	m_methods(m_context, m_presentation),
	m_interface(m_context, m_presentation),
	m_framedraw(m_context, m_presentation, m_methods)
{

}

Renderer::~Renderer() noexcept {

}

void Renderer::render() {
	if (!prepareFrame())
		configure_presentation();
		return;

	renderFrame();
	endFrame();
}

bool Renderer::prepareFrame() {
	acquiredImage = m_interface.acquire_toWriteImage();

	if (acquiredImage == -1)
		return false;

	return true;
}

void Renderer::renderFrame() {
	m_framedraw.drawFrame(acquiredImage, m_interface.get_activeFrame_cmd());
	m_interface.submit_activeFrame_cmd();
}

void Renderer::endFrame() {
	m_interface.present_activeFrame(acquiredImage);
	m_interface.advance_frame();
}

void Renderer::configure_presentation() {
	m_presentation.refresh_swapchain();
}
