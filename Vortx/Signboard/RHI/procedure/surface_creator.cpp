#include "surface_creator.h"

#include "Signboard/RHI/core/instance_vk.h"
#include "Signboard/RHI/core/surface_vk.h"

#include "Signboard/Platform/window/primitive/display_window_glfwAccess.h"

#include <stdexcept>

namespace rhi::procedure {

	surface_creator::surface_creator(const rhi::core::instance& instance)
		: m_instance(instance) {}

	rhi::core::surface surface_creator::create_surface(const window::primitive::display_window& w) {
		VkInstance l_instance = rhi::core::instance_vkAccess::get(m_instance);
		GLFWwindow* l_window = window::primitive::display_window_glfwAccess::get(w);

		VkSurfaceKHR l_surface = VK_NULL_HANDLE;
		if (glfwCreateWindowSurface(l_instance, l_window, nullptr, &l_surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vulkan surface!");
		
		rhi::core::surface surface;
		surface.m_instance = l_instance;
		surface.m_surface = l_surface;
		return surface;
	}
}