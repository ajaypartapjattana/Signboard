#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdWatchdog::pcdWatchdog(const device& device) noexcept
		:
		r_device(_pAccess::extract(device))
	{
		
	}

	void pcdWatchdog::watch_fence(const pmvFence& fence) const noexcept {
		VkFence a_fence = _pAccess::extract(fence);

		vkWaitForFences(r_device, 1, &a_fence, VK_TRUE, UINT64_MAX);
	}

	void pcdWatchdog::reset_fence(const pmvFence& fence) const noexcept {
		VkFence a_fence = _pAccess::extract(fence);

		vkResetFences(r_device, 1, &a_fence);
	}

	void pcdWatchdog::wait_device() const noexcept {
		vkDeviceWaitIdle(r_device);
	}

}