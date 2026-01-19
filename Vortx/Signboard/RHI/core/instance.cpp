#include "instance.h"

#include <vulkan/vulkan.h>

namespace rhi::core {

	instance::instance(instance&& other) noexcept {

		m_instance = other.m_instance;
		m_allocator = other.m_allocator;

		other.m_instance = nullptr;
		other.m_allocator = nullptr;
	}

	instance& instance::operator=(instance&& other) noexcept {
		
		if (this == &other)
			return *this;

		if (m_instance) {
			vkDestroyInstance(
				reinterpret_cast<VkInstance>(m_instance),
				reinterpret_cast<VkAllocationCallbacks*>(m_allocator)
			);
		}

		m_instance = other.m_instance;
		m_allocator = other.m_allocator;

		other.m_instance = nullptr;
		other.m_allocator = nullptr;

		return *this;
	}

	instance::~instance() noexcept {
	
		if (m_instance) {
			vkDestroyInstance(
				reinterpret_cast<VkInstance>(m_instance),
				reinterpret_cast<VkAllocationCallbacks*>(m_allocator)
			);
		}
	}

	const void* instance::native_instance() const noexcept {
		return m_instance;
	}

}
