#include "_watchdog.h"

#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/primitive/fence_vk.h"

namespace rhi::procedure {

	_watchdog::_watchdog(const rhi::core::device& device) noexcept 
		:
		r_device(rhi::core::device_vkAccess::get(device))
	{
		
	}

	void _watchdog::watch_fence(const rhi::primitive::fence& fence) const noexcept {
		VkFence a_fence = rhi::primitive::fence_vkAccess::get(fence);

		vkWaitForFences(r_device, 1, &a_fence, VK_TRUE, UINT64_MAX);
	}

	void _watchdog::reset_fence(const rhi::primitive::fence& fence) const noexcept {
		VkFence a_fence = rhi::primitive::fence_vkAccess::get(fence);

		vkResetFences(r_device, 1, &a_fence);
	}

	void _watchdog::wait_device() const noexcept {
		vkDeviceWaitIdle(r_device);
	}

}