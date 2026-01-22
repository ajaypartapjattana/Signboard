#include "device.h"

#include <vulkan/vulkan.h>

namespace rhi::core {

	device::device(device&& other) noexcept {
		m_device = other.m_device;
		m_physical = other.m_physical;
		m_allocator = other.m_allocator;
		m_queues = other.m_queues;

		other.m_device = VK_NULL_HANDLE;
		other.m_physical = VK_NULL_HANDLE;
		other.m_allocator = nullptr;
		other.m_queues.clear();
	}

	device& device::operator=(device&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_device)
			vkDestroyDevice(m_device, m_allocator);

		m_device = other.m_device;
		m_physical = other.m_physical;
		m_allocator = other.m_allocator;
		m_queues = other.m_queues;

		other.m_device = VK_NULL_HANDLE;
		other.m_physical = VK_NULL_HANDLE;
		other.m_allocator = nullptr;
		other.m_queues.clear();

		return *this;
	}

	device::~device() noexcept {
		if (m_device != VK_NULL_HANDLE)
			vkDestroyDevice(m_device, m_allocator);
	}

	const VkDevice* device::native_device() const noexcept {
		return &m_device;
	}

}