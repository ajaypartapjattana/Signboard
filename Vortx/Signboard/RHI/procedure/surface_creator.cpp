#include "surface_creator.h"

#include "Signboard/RHI/core/surface.h"
#include "Signboard/RHI/core/instance_vk.h"

#include "Signboard/Platform/primitive/display_window_glfwAccess.h"

namespace rhi::procedure {

	surface_creator::surface_creator(const rhi::core::instance& instance)
		: m_instance(rhi::core::instance_vkAccess::get(instance)) 
	{
		
	}

	VkResult surface_creator::create_surface(const platform::primitive::display_window& w, rhi::core::surface& target_surface) const {
		GLFWwindow* l_window = platform::primitive::display_window_glfwAccess::get(w);

		VkSurfaceKHR l_surface = VK_NULL_HANDLE;
		VkResult result = glfwCreateWindowSurface(m_instance, l_window, nullptr, &l_surface);
		if (result != VK_SUCCESS)
			return result;
		
		target_surface.m_instance = m_instance;
		target_surface.m_surface = l_surface;

		return result;

	}
}