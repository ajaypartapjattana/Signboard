#include "renderer.h"

Renderer::Renderer(const platform::primitive::display_window& render_target) 
	: m_renderInterface(render_target), m_passes(m_renderInterface), 
{

}

bool Renderer::prepareFrame() {
	return true;
}

void Renderer::renderFrame() {
	
}

void Renderer::endFrame() {

}