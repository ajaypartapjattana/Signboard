#include "queueSubmission.h"

#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/commandBuffer_pAccess.h"
#include "Signboard/RHI/primitive/semaphore_pAccess.h"
#include "Signboard/RHI/primitive/fence_pAccess.h"

static constexpr uint32_t MAX_WAIT = 4;
static constexpr uint32_t MAX_SIGNAL = 4;
static constexpr uint32_t MAX_CMD = 4;

namespace rhi {

	pcdQueueSubmission::pcdQueueSubmission(const rhi::creDevice& device) noexcept
		:
		r_queues(rhi::access::device_pAccess::get_queues(device)),
		toTrigger_fence(VK_NULL_HANDLE)
	{
		waitSemaphores.reserve(4);
		waitStages.reserve(4);
		signalSemaphores.reserve(4);
		cmdBuffers.reserve(4);

		info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	}

	void pcdQueueSubmission::reset() noexcept {
		waitSemaphores.clear();
		waitStages.clear();
		signalSemaphores.clear();
		cmdBuffers.clear();
		toTrigger_fence = VK_NULL_HANDLE;
	}

	pcdQueueSubmission& pcdQueueSubmission::add_wait(const rhi::pmvSemaphore& sem, VkPipelineStageFlags stage) noexcept {
		waitSemaphores.push_back(rhi::access::semaphore_pAccess::get(sem));
		waitStages.push_back(stage);

		return *this;
	}

	pcdQueueSubmission& pcdQueueSubmission::add_signal(const rhi::pmvSemaphore& sem) noexcept {
		signalSemaphores.push_back(rhi::access::semaphore_pAccess::get(sem));
		
		return *this;
	}

	pcdQueueSubmission& pcdQueueSubmission::set_toTrigger_fence(const rhi::pmvFence& fence) noexcept {
		toTrigger_fence = rhi::access::fence_pAccess::get(fence);
		return *this;
	}

	pcdQueueSubmission& pcdQueueSubmission::add_cmd(const rhi::pmvCommandBuffer& cmd) noexcept {
		cmdBuffers.push_back(rhi::access::commandBuffer_pAccess::get(cmd));

		return *this;
	}

	void pcdQueueSubmission::_prepare_submit() noexcept {
		info.pWaitSemaphores = waitSemaphores.data();
		info.pWaitDstStageMask = waitStages.data();
		info.pSignalSemaphores = signalSemaphores.data();
		info.pCommandBuffers = cmdBuffers.data();

		info.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
		info.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
		info.commandBufferCount = static_cast<uint32_t>(cmdBuffers.size());
	}

	VkResult pcdQueueSubmission::submit_graphics() {
		_prepare_submit();
		return vkQueueSubmit(r_queues.graphics , 1, &info, toTrigger_fence);
	}

	VkResult pcdQueueSubmission::submit_compute() {
		_prepare_submit();
		return vkQueueSubmit(r_queues.compute, 1, &info, toTrigger_fence);
	}

	VkResult pcdQueueSubmission::submit_transfer() {
		_prepare_submit();
		return vkQueueSubmit(r_queues.transfer, 1, &info, toTrigger_fence);
	}

	VkResult pcdQueueSubmission::submit_present() {
		_prepare_submit();
		return vkQueueSubmit(r_queues.present, 1, &info, toTrigger_fence);
	}

}