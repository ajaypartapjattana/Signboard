#include "swapchain_presenter.h"

#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/primitive/swapchain_vk.h"
#include "Signboard/RHI/primitive/semaphore_vkAccess.h"

namespace rhi::procedure {

	swapchain_presenter::swapchain_presenter(const rhi::core::device& device, const rhi::primitive::swapchain& swapchain) noexcept
		:
		r_presentQueue(rhi::core::device_vkAccess::get_queues(device).present),
		r_swapchain(rhi::primitive::swapchain_vkAccess::get(swapchain)),

		info()
	{
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.swapchainCount = 1;
		info.pSwapchains = &r_swapchain;
		info.pResults = nullptr;
	}

	swapchain_presenter::swapchain_presenter(swapchain_presenter&& other) noexcept 
		: 
		r_swapchain(other.r_swapchain),
		r_presentQueue(other.r_presentQueue),

		toWait_semaphores(std::move(other.toWait_semaphores)),
		info(other.info)
	{
		info.pSwapchains = &r_swapchain;
	}

	swapchain_presenter& swapchain_presenter::operator=(swapchain_presenter&& other) noexcept {
		if (this == &other)
			return *this;

		r_swapchain = other.r_swapchain;
		r_presentQueue = other.r_presentQueue;

		toWait_semaphores = std::move(other.toWait_semaphores);
		info = other.info;

		info.pSwapchains = &r_swapchain;

		return *this;
	}

	swapchain_presenter& swapchain_presenter::update_toWait_semaphores(const rhi::primitive::semaphore* pSemaphores, uint32_t count) {
		toWait_semaphores.resize(count);
		for (uint32_t i = 0; i < count; ++i) {
			toWait_semaphores[i] = rhi::primitive::semaphore_vkAccess::get(pSemaphores[i]);
		}

		info.pWaitSemaphores = toWait_semaphores.data();
		info.waitSemaphoreCount = count;

		return *this;
	}

	VkResult swapchain_presenter::present(uint32_t index) {
		info.pImageIndices = &index;
		return vkQueuePresentKHR(r_presentQueue, &info);
	}

}