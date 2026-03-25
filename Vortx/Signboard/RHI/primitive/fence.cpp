#include "fence.h"

#include "Signboard/RHI/core/device_pAccess.h"

namespace rhi {

	pmvFence::pmvFence(const rhi::creDevice& device) noexcept
		:
		m_fence(VK_NULL_HANDLE),
		m_device(rhi::access::device_pAccess::get(device))
	{
		create(false);
	}

	pmvFence::pmvFence(const rhi::creDevice& device, const bool signaled) noexcept
		:
		m_fence(VK_NULL_HANDLE),
		m_device(rhi::access::device_pAccess::get(device))
	{
		create(true);
	}

	void pmvFence::create(const bool signaled) {
		VkFenceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

		vkCreateFence(m_device, &createInfo, nullptr, &m_fence);
	}

	pmvFence::pmvFence(pmvFence&& other) noexcept
		:
		m_fence(other.m_fence),
		m_device(other.m_device)
	{
		other.m_fence = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
	}

	pmvFence& pmvFence::operator=(pmvFence&& other) noexcept {
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

	pmvFence::~pmvFence() noexcept {
		if (m_fence)
			vkDestroyFence(m_device, m_fence, nullptr);
	}

}