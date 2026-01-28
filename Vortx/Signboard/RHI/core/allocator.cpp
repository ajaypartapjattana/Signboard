#include "allocator.h"

namespace rhi::core {

	allocator::allocator(allocator&& other) noexcept {
		m_allocator = other.m_allocator;

		other.m_allocator = VK_NULL_HANDLE;
	}

	allocator& allocator::operator=(allocator&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_allocator)
			vmaDestroyAllocator(m_allocator);

		m_allocator = other.m_allocator;

		other.m_allocator = VK_NULL_HANDLE;

		return *this;
	}

	allocator::~allocator() noexcept {
		if (m_allocator)
			vmaDestroyAllocator(m_allocator);
	}

	VmaAllocator allocator::native_allocator() const noexcept {
		return m_allocator;
	}

}