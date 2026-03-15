#include "queue_submission.h"

#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/primitive/commandBuffer_vk.h"

namespace rhi::procedure {

	queue_submission::queue_submission(const rhi::core::device& device) noexcept
		:
		r_device(device)
	{

	}

	VkResult queue_submission::submit(std::vector<rhi::primitive::commandBuffer>& cmdBuffers) {
		std::vector<VkCommandBuffer> toSubmit_cmd;
		toSubmit_cmd.reserve(cmdBuffers.size());
		for (uint32_t i = 0; i < static_cast<uint32_t>(cmdBuffers.size()); ++i) {
			toSubmit_cmd[i] = cmdBuffers[i].native_commandBuffer();
		}
		
		VkSubmitInfo info{};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.waitSemaphoreCount = static_cast<uint32_t>(toWait_semaphores.size());
		info.pWaitSemaphores = toWait_semaphores.data();
		info.pWaitDstStageMask = toWait_stage.data();

		info.commandBufferCount = static_cast<uint32_t>(toSubmit_cmd.size());
		info.pCommandBuffers = toSubmit_cmd.data();

		info.signalSemaphoreCount = static_cast<uint32_t>(toSignal_semaphores.size());
		info.pSignalSemaphores = toSignal_semaphores.data();

		return vkQueueSubmit();

	}

}