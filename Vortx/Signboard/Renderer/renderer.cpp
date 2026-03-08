#include "renderer.h"

Renderer::Renderer(const platform::primitive::display_window& render_target) 
	: 
	m_context(render_target),
	m_interface(m_context, 2), 
	m_passes(m_context, m_interface), 
	m_materials(m_context, m_interface, m_passes)
{

}

bool Renderer::prepareFrame() {
	return true;
}

void Renderer::renderFrame() {
	
}

void Renderer::endFrame() {

}