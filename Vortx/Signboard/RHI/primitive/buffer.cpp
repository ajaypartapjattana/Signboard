#include "buffer.h"

namespace rhi {

	pmvBuffer::pmvBuffer() noexcept
		:
		m_buffer(VK_NULL_HANDLE),
		m_allocation(VK_NULL_HANDLE),
		m_allocator(VK_NULL_HANDLE)
	{

	}

	pmvBuffer::pmvBuffer(pmvBuffer&& other) noexcept 
		:
		m_buffer(other.m_buffer),
		m_allocation(other.m_allocation),
		m_allocator(other.m_allocator)
	{
		other.m_buffer = VK_NULL_HANDLE;
	}

	pmvBuffer& pmvBuffer::operator=(pmvBuffer&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_buffer)
			vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);

		m_buffer = other.m_buffer;
		m_allocation = other.m_allocation;
		m_allocator = other.m_allocator;

		other.m_buffer = VK_NULL_HANDLE;

		return *this;
	}

	pmvBuffer::~pmvBuffer() noexcept {
		if (m_buffer)
			vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
	}

}