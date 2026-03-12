#include "fence.h"

namespace rhi::primitive {

	fence::fence() noexcept
		:
		m_fence(VK_NULL_HANDLE),
		m_device(VK_NULL_HANDLE)
	{

	}

	fence::fence(fence&& other) noexcept
		:
		m_fence(other.m_fence),
		m_device(other.m_device)
	{
		other.m_fence = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
	}

	fence& fence::operator=(fence&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_fence)
			vkDestroyFence(m_device, m_fence, nullptr);

		m_fence = other.m_fence;
		m_device = other.m_device;

		other.m_fence = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;

		return *this;
	}

	fence::~fence() noexcept {
		if (m_fence)
			vkDestroyFence(m_device, m_fence, nullptr);
	}

}