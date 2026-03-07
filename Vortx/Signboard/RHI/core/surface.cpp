#include "surface.h"

#include <vulkan/vulkan.h>
#include <stdexcept>

#include "instance_vk.h"
#include "Signboard/Platform/primitive/display_window_glfwAccess.h"

namespace rhi::core {

	surface::surface(const platform::primitive::display_window& window, const instance& instance)
		: m_surface(VK_NULL_HANDLE), m_instance(rhi::core::instance_vkAccess::get(instance))
	{
		GLFWwindow* a_window = platform::primitive::display_window_glfwAccess::get(window);

		VkResult result = glfwCreateWindowSurface(m_instance, a_window, nullptr, &m_surface);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: surface_creation!");

	}

	surface::surface(surface&& other) noexcept {
		m_surface = other.m_surface;
		m_instance = other.m_instance;

		other.m_surface = VK_NULL_HANDLE;
		other.m_instance = VK_NULL_HANDLE;
	}

	surface& surface::operator=(surface&& other) noexcept {
		if (this == &other)
			return *this;

		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

		m_surface = other.m_surface;
		m_instance = other.m_instance;

		other.m_surface = VK_NULL_HANDLE;
		other.m_instance = VK_NULL_HANDLE;

		return *this;
	}

	surface::~surface() noexcept {
		if(m_surface != VK_NULL_HANDLE)
			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	}

	const VkSurfaceKHR* surface::native_surface() const noexcept {
		return &m_surface;
	}

}