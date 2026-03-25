#include "swapchainPresenter.h"

#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/swapchain_pAccess.h"
#include "Signboard/RHI/primitive/semaphore_pAccess.h"

namespace rhi {

	pcdSwapchainPresenter::pcdSwapchainPresenter(const rhi::creDevice& device, const rhi::pmvSwapchain& swapchain) noexcept
		:
		r_presentQueue(rhi::access::device_pAccess::get_queues(device).present),
		r_swapchain(rhi::access::swapchain_pAccess::get(swapchain)),

		info()
	{
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.swapchainCount = 1;
		info.pSwapchains = &r_swapchain;
		info.pResults = nullptr;
	}

	pcdSwapchainPresenter::pcdSwapchainPresenter(pcdSwapchainPresenter&& other) noexcept 
		: 
		r_swapchain(other.r_swapchain),
		r_presentQueue(other.r_presentQueue),

		toWait_semaphores(std::move(other.toWait_semaphores)),
		info(other.info)
	{
		info.pSwapchains = &r_swapchain;
	}

	pcdSwapchainPresenter& pcdSwapchainPresenter::operator=(pcdSwapchainPresenter&& other) noexcept {
		if (this == &other)
			return *this;

		r_swapchain = other.r_swapchain;
		r_presentQueue = other.r_presentQueue;

		toWait_semaphores = std::move(other.toWait_semaphores);
		info = other.info;

		info.pSwapchains = &r_swapchain;

		return *this;
	}

	pcdSwapchainPresenter& pcdSwapchainPresenter::update_toWait_semaphores(const rhi::pmvSemaphore* pSemaphores, uint32_t count) {
		toWait_semaphores.resize(count);
		for (uint32_t i = 0; i < count; ++i) {
			toWait_semaphores[i] = rhi::access::semaphore_pAccess::get(pSemaphores[i]);
		}

		info.pWaitSemaphores = toWait_semaphores.data();
		info.waitSemaphoreCount = count;

		return *this;
	}

	VkResult pcdSwapchainPresenter::present(uint32_t index) {
		info.pImageIndices = &index;
		return vkQueuePresentKHR(r_presentQueue, &info);
	}

}