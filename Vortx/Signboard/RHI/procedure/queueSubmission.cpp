#include "queueSubmission.h"

#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/commandBuffer_pAccess.h"
#include "Signboard/RHI/primitive/semaphore_pAccess.h"
#include "Signboard/RHI/primitive/fence_pAccess.h"

namespace rhi {

	pcdQueueSubmission::pcdQueueSubmission(const rhi::creDevice& device) noexcept
		:
		r_queues(rhi::access::device_pAccess::get_queues(device)),
		toTrigger_fence(VK_NULL_HANDLE)
	{
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	}

	void pcdQueueSubmission::reset() noexcept {
		toWait_semaphores.clear();
		toWait_stages.clear();
		toSignal_semaphores.clear();
	}

	pcdQueueSubmission& pcdQueueSubmission::update_toWait_semaphores(const rhi::pmvSemaphore* pSemaphores, const VkPipelineStageFlags* pWaitStages, uint32_t count) noexcept {
		toWait_semaphores.resize(count);
		toWait_stages.resize(count);
		for (uint32_t i = 0; i < count; i++) {
			toWait_semaphores[i] = rhi::access::semaphore_pAccess::get(pSemaphores[i]);
			toWait_stages[i] = pWaitStages[i];
		}

		info.waitSemaphoreCount = static_cast<uint32_t>(toWait_semaphores.size());
		info.pWaitSemaphores = toWait_semaphores.data();
		info.pWaitDstStageMask = toWait_stages.data();

		return *this;
	}

	pcdQueueSubmission& pcdQueueSubmission::update_toSignal_semaphores(const rhi::pmvSemaphore* pSemaphores, uint32_t count) noexcept {
		toSignal_semaphores.resize(count);
		for (uint32_t i = 0; i < count; ++i) {
			toSignal_semaphores[i] = rhi::access::semaphore_pAccess::get(pSemaphores[i]);
		}

		info.signalSemaphoreCount = static_cast<uint32_t>(toSignal_semaphores.size());
		info.pSignalSemaphores = toSignal_semaphores.data();
		
		return *this;
	}

	pcdQueueSubmission& pcdQueueSubmission::set_toTrigger_fence(const rhi::pmvFence& fence) noexcept {
		toTrigger_fence = rhi::access::fence_pAccess::get(fence);
		return *this;
	}

	pcdQueueSubmission& pcdQueueSubmission::update_toSubmit_cmd(const rhi::pmvCommandBuffer* cmdBuffers, uint32_t cmd_count) noexcept {
		toSubmit_cmd.resize(cmd_count);
		for (uint32_t i = 0; i < cmd_count; ++i) {
			toSubmit_cmd[i] = cmdBuffers[i].m_commandBuffer;
		}

		info.commandBufferCount = cmd_count;
		info.pCommandBuffers = toSubmit_cmd.data();

		return *this;
	}

	VkResult pcdQueueSubmission::submit_graphics_cmd() {
		return vkQueueSubmit(r_queues.graphics , 1, &info, toTrigger_fence);
	}

	VkResult pcdQueueSubmission::submit_compute_cmd() {
		return vkQueueSubmit(r_queues.compute, 1, &info, toTrigger_fence);
	}

	VkResult pcdQueueSubmission::submit_transfer_cmd() {
		return vkQueueSubmit(r_queues.transfer, 1, &info, toTrigger_fence);
	}

	VkResult pcdQueueSubmission::submit_present_cmd() {
		return vkQueueSubmit(r_queues.present, 1, &info, toTrigger_fence);
	}

}