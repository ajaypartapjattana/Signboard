#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdQueuePresent::pcdQueuePresent(const device& device, VkPresentInfoKHR* pCreateInfo) noexcept
		:
		r_presentQueue(_pAccess::queues(device).present),
		pInfo(allot_basic(pCreateInfo))
	{

	}

	VkPresentInfoKHR* pcdQueuePresent::allot_basic(VkPresentInfoKHR* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<VkPresentInfoKHR>();

		VkPresentInfoKHR* _info = m_ownedInfo.get();
		_info->sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		return _info;
	}

	void pcdQueuePresent::target_swapchains(sgb::span<const swapchain> swapchains) noexcept {
		size_t _sSz = swapchains.size();

		r_swapchains.clear();
		r_swapchains.reserve(_sSz);
		for (size_t i = 0; i < _sSz; ++i) {
			r_swapchains.emplace_back(_pAccess::extract(swapchains[i]));
		}

		pInfo->pSwapchains = r_swapchains.data();
		pInfo->swapchainCount = static_cast<uint32_t>(_sSz);
	}

	void pcdQueuePresent::target_waitSemaphores(sgb::span<const VkSemaphore> semaphores) noexcept {
		pInfo->waitSemaphoreCount = static_cast<uint32_t>(semaphores.size());
		pInfo->pWaitSemaphores = semaphores.data();
	}

	VkResult pcdQueuePresent::present(uint32_t index) noexcept {
		pInfo->pImageIndices = &index;
		return vkQueuePresentKHR(r_presentQueue, pInfo);
	}

	void pcdQueuePresent::preset(VkPresentInfoKHR* pPresentInfo) noexcept {
		m_ownedInfo.reset();
		pInfo = allot_basic(pPresentInfo);
	}

	void pcdQueuePresent::reset() noexcept {
		m_ownedInfo.reset();
		pInfo = allot_basic(nullptr);
	}

}