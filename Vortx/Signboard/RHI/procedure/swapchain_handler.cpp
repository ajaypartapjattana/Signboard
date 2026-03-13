#include "swapchain_handler.h"

#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/primitive/swapchain_vk.h"

#include "Signboard/RHI/primitive/semaphore_vkAccess.h"

namespace rhi::procedure {

	swapchain_handler::swapchain_handler(const rhi::core::device& device) noexcept
		:
		r_device(rhi::core::device_vkAccess::get(device)),
		r_swapchain(VK_NULL_HANDLE)
	{

	}

	void swapchain_handler::set_swapchain(const rhi::primitive::swapchain& swapchain) noexcept {
		r_swapchain = rhi::primitive::swapchain_vkAccess::get(swapchain);
	}

	void swapchain_handler::acquire_freeSwapchainImage(const rhi::primitive::semaphore* semaphore, uint32_t& index) const noexcept {
		VkSemaphore a_semaphore = semaphore ? rhi::primitive::semaphore_vkAccess::get(*semaphore) : VK_NULL_HANDLE;
		vkAcquireNextImageKHR(r_device, r_swapchain, UINT64_MAX, a_semaphore, VK_NULL_HANDLE, &index);
	}



}