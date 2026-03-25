#include "watchdog.h"

#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/fence_pAccess.h"

namespace rhi {

	pcdWatchdog::pcdWatchdog(const rhi::creDevice& device) noexcept
		:
		r_device(rhi::access::device_pAccess::get(device))
	{
		
	}

	void pcdWatchdog::watch_fence(const rhi::pmvFence& fence) const noexcept {
		VkFence a_fence = rhi::access::fence_pAccess::get(fence);

		vkWaitForFences(r_device, 1, &a_fence, VK_TRUE, UINT64_MAX);
	}

	void pcdWatchdog::reset_fence(const rhi::pmvFence& fence) const noexcept {
		VkFence a_fence = rhi::access::fence_pAccess::get(fence);

		vkResetFences(r_device, 1, &a_fence);
	}

	void pcdWatchdog::wait_device() const noexcept {
		vkDeviceWaitIdle(r_device);
	}

}