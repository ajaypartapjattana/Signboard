#include "swapchainHandler.h"

#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/swapchain_pAccess.h"

#include "Signboard/RHI/primitive/semaphore_pAccess.h"

namespace rhi {

	pcdSwapchainHandler::pcdSwapchainHandler(const rhi::creDevice& device, const rhi::pmvSwapchain& swapchain) noexcept
		:
		r_device(rhi::access::device_pAccess::get(device)),
		r_swapchain(rhi::access::swapchain_pAccess::get(swapchain))
	{

	}

	pcdSwapchainHandler::pcdSwapchainHandler(pcdSwapchainHandler&& other) noexcept 
		: 
		r_swapchain(other.r_swapchain),
		r_device(other.r_device)
	{

	}

	pcdSwapchainHandler& pcdSwapchainHandler::operator=(pcdSwapchainHandler&& other) noexcept {
		if (this == &other)
			return *this;

		r_swapchain = other.r_swapchain;
		r_device = other.r_device;

		return *this;
	}

	VkResult pcdSwapchainHandler::acquire_freeSwapchainImage(const rhi::pmvSemaphore* semaphore, uint32_t& index) const noexcept {
		VkSemaphore a_semaphore = semaphore ? rhi::access::semaphore_pAccess::get(*semaphore) : VK_NULL_HANDLE;
		return vkAcquireNextImageKHR(r_device, r_swapchain, UINT64_MAX, a_semaphore, VK_NULL_HANDLE, &index);
	}

}