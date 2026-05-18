#include "Signboard/RHI/Internal/rhi_pAccess.h"

// an improved device queue storing system can remove the need for queue selection each time submitting

namespace rhi {

	pcdQueueSubmit::pcdQueueSubmit(VkSubmitInfo* pSubmitInfo) noexcept
		:
		pInfo(allot_basic(pSubmitInfo))
	{

	}

	VkSubmitInfo* pcdQueueSubmit::allot_basic(VkSubmitInfo* pSubmitInfo) noexcept {
		if (pSubmitInfo)
			return pSubmitInfo;

		m_ownedInfo = std::make_unique<VkSubmitInfo>();

		VkSubmitInfo* _info = m_ownedInfo.get();
		_info->sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		return _info;
	}

	void pcdQueueSubmit::target_waitSemaphores(sgb::span<const pmvSemaphore> semaphores, sgb::span<const VkPipelineStageFlags> stages) noexcept {
		size_t _sSz = semaphores.size();

		waitSemaphores.clear();
		waitSemaphores.reserve(_sSz);
		for (size_t i = 0; i < _sSz; ++i) {
			waitSemaphores.emplace_back(_pAccess::extract(semaphores[i]));
		}

		pInfo->waitSemaphoreCount = static_cast<uint32_t>(_sSz);
		pInfo->pWaitSemaphores = waitSemaphores.data();
		pInfo->pWaitDstStageMask = stages.data();
	}

	void pcdQueueSubmit::target_signalSemaphores(sgb::span<const pmvSemaphore> semaphores) noexcept {
		size_t _sSz = semaphores.size();

		signalSemaphores.clear();
		signalSemaphores.reserve(_sSz);
		for (size_t i = 0; i < _sSz; ++i) {
			signalSemaphores.emplace_back(_pAccess::extract(semaphores[i]));
		}

		pInfo->signalSemaphoreCount = static_cast<uint32_t>(_sSz);
		pInfo->pSignalSemaphores = signalSemaphores.data();
	}

	void pcdQueueSubmit::target_commandBuffers(sgb::span<const pmvCommandBuffer> commandBuffers) noexcept {
		size_t _sSz = commandBuffers.size();

		buffers.clear();
		buffers.reserve(_sSz);
		for (size_t i = 0; i < _sSz; ++i) {
			buffers.emplace_back(_pAccess::extract(commandBuffers[i]));
		}

		pInfo->commandBufferCount = static_cast<uint32_t>(_sSz);
		pInfo->pCommandBuffers = buffers.data();
	}

	VkResult pcdQueueSubmit::submit(VkQueue queue, const pmvFence& fence) const noexcept {
		return vkQueueSubmit(queue, 1, pInfo, _pAccess::extract(fence));
	}

	void pcdQueueSubmit::preset(VkSubmitInfo* pSubmitInfo) noexcept {
		m_ownedInfo.reset();
		pInfo = allot_basic(pSubmitInfo);
	}

	void pcdQueueSubmit::reset() noexcept {
		m_ownedInfo.reset();
		pInfo = allot_basic(nullptr);
	}

}