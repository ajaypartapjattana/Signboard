#include "swapchainHandler.h"

#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/swapchain_pAccess.h"

#include "Signboard/RHI/primitive/semaphore_pAccess.h"

namespace rhi {

	pcdSwapchainImageAcquire::pcdSwapchainImageAcquire(const creDevice& device) noexcept
		:
		r_device(access::device_pAccess::extract(device)),
		r_swapchain()
	{

	}

	pcdSwapchainImageAcquire::pcdSwapchainImageAcquire(pcdSwapchainImageAcquire&& other) noexcept 
		: 
		r_swapchain(other.r_swapchain),
		r_device(other.r_device)
	{

	}

	void pcdSwapchainImageAcquire::target_swapchain(const pmvSwapchain& swapchain) noexcept {
		r_swapchain = access::swapchain_pAccess::extract(swapchain);
	}

	VkResult pcdSwapchainImageAcquire::acquire_freeSwapchainImage(const pmvSemaphore* semaphore, uint32_t& index) const noexcept {
		VkSemaphore a_semaphore = semaphore ? access::semaphore_pAccess::get(*semaphore) : VK_NULL_HANDLE;
		return vkAcquireNextImageKHR(r_device, r_swapchain, UINT64_MAX, a_semaphore, VK_NULL_HANDLE, &index);
	}

}