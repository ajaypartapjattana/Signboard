#include "fence.h"

#include "Signboard/RHI/core/device_vk.h"

namespace rhi::primitive {

	fence::fence(const rhi::core::device& device) noexcept
		:
		m_fence(VK_NULL_HANDLE),
		m_device(rhi::core::device_vkAccess::get(device))
	{
		create(false);
	}

	fence::fence(const rhi::core::device& device, const bool signaled) noexcept
		:
		m_fence(VK_NULL_HANDLE),
		m_device(rhi::core::device_vkAccess::get(device))
	{
		create(true);
	}

	void fence::create(const bool signaled) {
		VkFenceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

		vkCreateFence(m_device, &createInfo, nullptr, &m_fence);
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