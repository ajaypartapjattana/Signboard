#include "surface.h"

#include <vulkan/vulkan.h>

namespace rhi::core {

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