#include "queue_submission.h"

#include "Signboard/RHI/primitive/commandBuffer_vk.h"
#include "Signboard/RHI/primitive/semaphore_vkAccess.h"
#include "Signboard/RHI/primitive/fence_vk.h"

namespace rhi::procedure {

	queue_submission::queue_submission(const rhi::core::device& device) noexcept
		:
		r_queues(device.m_queues),
		toTrigger_fence(VK_NULL_HANDLE)
	{
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	}

	void queue_submission::reset() noexcept {
		toWait_semaphores.clear();
		toWait_stages.clear();
		toSignal_semaphores.clear();
	}

	queue_submission& queue_submission::update_toWait_semaphores(const rhi::primitive::semaphore* pSemaphores, const VkPipelineStageFlags* pWaitStages, uint32_t count) noexcept {
		toWait_semaphores.resize(count);
		toWait_stages.resize(count);
		for (uint32_t i = 0; i < count; i++) {
			toWait_semaphores[i] = rhi::primitive::semaphore_vkAccess::get(pSemaphores[i]);
			toWait_stages[i] = pWaitStages[i];
		}

		info.waitSemaphoreCount = static_cast<uint32_t>(toWait_semaphores.size());
		info.pWaitSemaphores = toWait_semaphores.data();
		info.pWaitDstStageMask = toWait_stages.data();

		return *this;
	}

	queue_submission& queue_submission::update_toSignal_semaphores(const rhi::primitive::semaphore* pSemaphores, uint32_t count) noexcept {
		toSignal_semaphores.resize(count);
		for (uint32_t i = 0; i < count; ++i) {
			toSignal_semaphores[i] = rhi::primitive::semaphore_vkAccess::get(pSemaphores[i]);
		}

		info.signalSemaphoreCount = static_cast<uint32_t>(toSignal_semaphores.size());
		info.pSignalSemaphores = toSignal_semaphores.data();
		
		return *this;
	}

	queue_submission& queue_submission::set_toTrigger_fence(const rhi::primitive::fence& fence) noexcept {
		toTrigger_fence = rhi::primitive::fence_vkAccess::get(fence);
		return *this;
	}

	queue_submission& queue_submission::update_toSubmit_cmd(const rhi::primitive::commandBuffer* cmdBuffers, uint32_t cmd_count) noexcept {
		toSubmit_cmd.resize(cmd_count);
		for (uint32_t i = 0; i < cmd_count; ++i) {
			toSubmit_cmd[i] = cmdBuffers[i].native_commandBuffer();
		}

		info.commandBufferCount = cmd_count;
		info.pCommandBuffers = toSubmit_cmd.data();

		return *this;
	}

	VkResult queue_submission::submit_graphics_cmd() {
		return vkQueueSubmit(r_queues.graphics , 1, &info, toTrigger_fence);
	}

	VkResult queue_submission::submit_compute_cmd() {
		return vkQueueSubmit(r_queues.compute, 1, &info, toTrigger_fence);
	}

	VkResult queue_submission::submit_transfer_cmd() {
		return vkQueueSubmit(r_queues.transfer, 1, &info, toTrigger_fence);
	}

	VkResult queue_submission::submit_present_cmd() {
		return vkQueueSubmit(r_queues.present, 1, &info, toTrigger_fence);
	}

}