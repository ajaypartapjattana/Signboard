#include "swapchain_handler.h"

#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/primitive/swapchain_vk.h"

#include "Signboard/RHI/primitive/semaphore_vkAccess.h"

namespace rhi::procedure {

	swapchain_handler::swapchain_handler(const rhi::core::device& device, const rhi::primitive::swapchain& swapchain) noexcept
		:
		r_device(rhi::core::device_vkAccess::get(device)),
		r_swapchain(rhi::primitive::swapchain_vkAccess::get(swapchain))
	{

	}

	swapchain_handler::swapchain_handler(swapchain_handler&& other) noexcept 
		: 
		r_swapchain(other.r_swapchain),
		r_device(other.r_device)
	{

	}

	swapchain_handler& swapchain_handler::operator=(swapchain_handler&& other) noexcept {
		if (this == &other)
			return *this;

		r_swapchain = other.r_swapchain;
		r_device = other.r_device;

		return *this;
	}

	VkResult swapchain_handler::acquire_freeSwapchainImage(const rhi::primitive::semaphore* semaphore, uint32_t& index) const noexcept {
		VkSemaphore a_semaphore = semaphore ? rhi::primitive::semaphore_vkAccess::get(*semaphore) : VK_NULL_HANDLE;
		return vkAcquireNextImageKHR(r_device, r_swapchain, UINT64_MAX, a_semaphore, VK_NULL_HANDLE, &index);
	}

}