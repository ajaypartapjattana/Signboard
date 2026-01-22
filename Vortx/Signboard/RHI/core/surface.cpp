#include "surface.h"

#include <vulkan/vulkan.h>

namespace rhi::core {

	surface::surface(surface&& other) noexcept {
		m_surface = other.m_surface;
		m_instance = other.m_instance;
		m_allocator = other.m_allocator;

		other.m_surface = VK_NULL_HANDLE;
		other.m_instance = VK_NULL_HANDLE;
		other.m_allocator = nullptr;
	}

	surface& surface::operator=(surface&& other) noexcept {
		vkDestroySurfaceKHR(m_instance, m_surface, m_allocator);

		m_surface = other.m_surface;
		m_instance = other.m_instance;
		m_allocator = other.m_allocator;

		other.m_surface = VK_NULL_HANDLE;
		other.m_instance = VK_NULL_HANDLE;
		other.m_allocator = nullptr;
	}

	surface::~surface() noexcept {
		if(m_surface != VK_NULL_HANDLE)
			vkDestroySurfaceKHR(m_instance, m_surface, m_allocator);
	}

	const VkSurfaceKHR* surface::native_surface() const noexcept {
		return &m_surface;
	}

}