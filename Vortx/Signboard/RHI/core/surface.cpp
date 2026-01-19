#include "surface.h"

#include <vulkan/vulkan.h>

namespace rhi::core {

	surface::surface(surface&& other) noexcept {

		m_surface = other.m_surface;
		m_instance = other.m_instance;
		m_allocator = other.m_allocator;

		other.m_surface = nullptr;
		other.m_instance = nullptr;
		other.m_allocator = nullptr;
	}

	surface& surface::operator=(surface&& other) noexcept {
		
		vkDestroySurfaceKHR(
			reinterpret_cast<VkInstance>(m_instance),
			reinterpret_cast<VkSurfaceKHR>(m_surface),
			reinterpret_cast<VkAllocationCallbacks*>(m_allocator)
		);

		m_surface = other.m_surface;
		m_instance = other.m_instance;
		m_allocator = other.m_allocator;

		other.m_surface = nullptr;
		other.m_instance = nullptr;
		other.m_allocator = nullptr;
	}

	surface::~surface() noexcept {
		
		vkDestroySurfaceKHR(
			reinterpret_cast<VkInstance>(m_instance),
			reinterpret_cast<VkSurfaceKHR>(m_surface),
			reinterpret_cast<VkAllocationCallbacks*>(m_allocator)
		);
	}

	const void* surface::native_surface() const noexcept {
		return m_surface;
	}

}