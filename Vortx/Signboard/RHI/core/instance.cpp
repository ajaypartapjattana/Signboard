#include "instance.h"

#include <vulkan/vulkan.h>

namespace rhi::core {

	instance::instance(instance&& other) noexcept {
		m_instance = other.m_instance;

		other.m_instance = VK_NULL_HANDLE;
	}

	instance& instance::operator=(instance&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_instance)
			vkDestroyInstance(m_instance, nullptr);

		m_instance = other.m_instance;

		other.m_instance = VK_NULL_HANDLE;

		return *this;
	}

	instance::~instance() noexcept {
		if (m_instance != VK_NULL_HANDLE)
			vkDestroyInstance(m_instance, nullptr);
	}

	const VkInstance* instance::native_instance() const noexcept {
		return &m_instance;
	}

}
