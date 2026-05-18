#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pmvFence::pmvFence(const creDevice& device) noexcept
		:
		m_fence(VK_NULL_HANDLE),
		r_device(_pAccess::extract(device))
	{
		create(false);
	}

	pmvFence::pmvFence(const creDevice& device, const bool signaled) noexcept
		:
		m_fence(VK_NULL_HANDLE),
		r_device(_pAccess::extract(device))
	{
		create(true);
	}

	void pmvFence::create(const bool signaled) {
		VkFenceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

		vkCreateFence(r_device, &createInfo, nullptr, &m_fence);
	}

	pmvFence::pmvFence(pmvFence&& other) noexcept
		:
		m_fence(other.m_fence),
		r_device(other.r_device)
	{
		other.m_fence = VK_NULL_HANDLE;
		other.r_device = VK_NULL_HANDLE;
	}

	pmvFence& pmvFence::operator=(pmvFence&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_fence)
			vkDestroyFence(r_device, m_fence, nullptr);

		m_fence = other.m_fence;
		r_device = other.r_device;

		other.m_fence = VK_NULL_HANDLE;
		other.r_device = VK_NULL_HANDLE;

		return *this;
	}

	pmvFence::~pmvFence() noexcept {
		if (m_fence)
			vkDestroyFence(r_device, m_fence, nullptr);
	}

}