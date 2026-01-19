#include "device.h"

#include <vulkan/vulkan.h>

namespace rhi::core {

	device::device(device&& other) noexcept {
		
		m_device = other.m_device;
		m_physical = other.m_physical;
		m_queues = other.m_queues;
		m_allocator = other.m_allocator;

		other.m_device = nullptr;
		other.m_physical = nullptr;
		other.m_queues.clear();
		other.m_allocator = nullptr;
	}

	device& device::operator=(device&& other) noexcept {

		if (this == &other)
			return *this;

		if (m_device) {
			vkDestroyDevice(
				reinterpret_cast<VkDevice>(m_device),
				reinterpret_cast<VkAllocationCallbacks*>(m_allocator)
			);
		}

		m_device = other.m_device;
		m_physical = other.m_physical;
		m_queues = other.m_queues;
		m_allocator = other.m_allocator;

		other.m_device = nullptr;
		other.m_physical = nullptr;
		other.m_queues.clear();
		other.m_allocator = nullptr;

		return *this;
	}

	device::~device() noexcept {

		if (m_device) {
			vkDestroyDevice(
				reinterpret_cast<VkDevice>(m_device),
				reinterpret_cast<VkAllocationCallbacks*>(m_allocator)
			);
		}
	}

	const void* device::native_device() const noexcept {
		return m_device;
	}

}