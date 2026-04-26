#include "swapchainPresenter.h"

#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/swapchain_pAccess.h"
#include "Signboard/RHI/primitive/semaphore_pAccess.h"

namespace rhi {

	pcdQueuePresent::pcdQueuePresent(const creDevice& device, const VkPresentInfoKHR* pCreateInfo) noexcept
		:
		r_presentQueue(access::device_pAccess::get_queues(device).present),
		r_swapchain(VK_NULL_HANDLE),
		_info(fetch_basic(pCreateInfo))
	{

	}

	VkPresentInfoKHR pcdQueuePresent::fetch_basic(const VkPresentInfoKHR* pCreateInfo) const noexcept {
		if (pCreateInfo)
			return *pCreateInfo;

		VkPresentInfoKHR info{};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.swapchainCount = 1;
		info.pSwapchains = &r_swapchain;

		return info;
	}

	pcdQueuePresent::pcdQueuePresent(pcdQueuePresent&& other) noexcept 
		: 
		r_presentQueue(other.r_presentQueue),
		r_swapchain(other.r_swapchain),

		waitSemaphores(std::move(other.waitSemaphores)),
		_info(other._info)
	{
		_info.pSwapchains = &r_swapchain;
	}

	void pcdQueuePresent::target_swapchain(const pmvSwapchain& swapchain) noexcept {
		r_swapchain = access::swapchain_pAccess::extract(swapchain);
	}

	pcdQueuePresent& pcdQueuePresent::add_wait(const rhi::pmvSemaphore* pSemaphores, uint32_t count) {
		waitSemaphores.resize(count);
		for (uint32_t i = 0; i < count; ++i) {
			waitSemaphores[i] = rhi::access::semaphore_pAccess::get(pSemaphores[i]);
		}

		_info.pWaitSemaphores = waitSemaphores.data();
		_info.waitSemaphoreCount = count;

		return *this;
	}

	VkResult pcdQueuePresent::present(uint32_t index) {
		_info.pImageIndices = &index;
		return vkQueuePresentKHR(r_presentQueue, &_info);
	}

}