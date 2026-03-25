#include "surface.h"

#include <stdexcept>

#include "instance_pAccess.h"
#include "Signboard/Platform/primitive/display_window_glfwAccess.h"

namespace rhi {

	creSurface::creSurface(const platform::primitive::displayWindow& window, const creInstance& instance)
		: 
		m_surface(VK_NULL_HANDLE), 
		m_instance(rhi::access::instance_pAccess::get(instance))
	{
		GLFWwindow* a_window = platform::primitive::displayWindow_pAccess::get(window);

		VkResult result = glfwCreateWindowSurface(m_instance, a_window, nullptr, &m_surface);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: surface_creation!");

	}

	creSurface::creSurface(creSurface&& other) noexcept 
		:
		m_surface(other.m_surface),
		m_instance(other.m_instance)
	{
		other.m_surface = VK_NULL_HANDLE;
	}

	creSurface& creSurface::operator=(creSurface&& other) noexcept {
		if (this == &other)
			return *this;
		
		if(m_surface)
			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

		m_surface = other.m_surface;
		m_instance = other.m_instance;

		other.m_surface = VK_NULL_HANDLE;

		return *this;
	}

	creSurface::~creSurface() noexcept {
		if(m_surface)
			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	}

}