#include "buffer.h"

namespace rhi::primitive {

	buffer::buffer(buffer&& other) noexcept {
		m_buffer = other.m_buffer;
		m_allocation = other.m_allocation;
		m_allocator = other.m_allocator;

		other.m_buffer = VK_NULL_HANDLE;
		other.m_allocation = VK_NULL_HANDLE;
		other.m_allocator = VK_NULL_HANDLE;
	}

	buffer& buffer::operator=(buffer&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_buffer)
			vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);

		m_buffer = other.m_buffer;
		m_allocation = other.m_allocation;
		m_allocator = other.m_allocator;

		other.m_buffer = VK_NULL_HANDLE;
		other.m_allocation = VK_NULL_HANDLE;
		other.m_allocator = VK_NULL_HANDLE;

		return *this;
	}

	buffer::~buffer() noexcept {
		if (m_buffer)
			vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
	}

	VkBuffer buffer::native_buffer() const noexcept {
		return m_buffer;
	}

}