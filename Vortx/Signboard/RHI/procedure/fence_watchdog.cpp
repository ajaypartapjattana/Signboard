#include "fence_watchdog.h"

#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/primitive/fence_vk.h"

namespace rhi::procedure {

	fence_watchdog::fence_watchdog(const rhi::core::device& device) noexcept 
		:
		r_device(rhi::core::device_vkAccess::get(device))
	{
		
	}

	void fence_watchdog::watch(const rhi::primitive::fence& fence) {
		VkFence a_fence = rhi::primitive::fence_vkAccess::get(fence);

		vkWaitForFences(r_device, 1, &a_fence, VK_TRUE, UINT64_MAX);
		vkResetFences(r_device, 1, &a_fence);
	}



}