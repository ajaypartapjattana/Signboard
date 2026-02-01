#include "surface_creator.h"

#include "Signboard/RHI/core/surface.h"
#include "Signboard/RHI/core/instance_vk.h"

#include "Signboard/Platform/primitive/display_window_glfwAccess.h"

#include <stdexcept>

namespace rhi::procedure {

	surface_creator::surface_creator(const rhi::core::instance& instance)
		: m_instance(rhi::core::instance_vkAccess::get(instance)) {}

	rhi::core::surface surface_creator::create_surface(const platform::primitive::display_window& w) {
		GLFWwindow* l_window = platform::primitive::display_window_glfwAccess::get(w);

		VkSurfaceKHR l_surface = VK_NULL_HANDLE;
		if (glfwCreateWindowSurface(m_instance, l_window, nullptr, &l_surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vulkan surface!");
		
		rhi::core::surface surface;
		surface.m_instance = m_instance;
		surface.m_surface = l_surface;
		return surface;
	}
}