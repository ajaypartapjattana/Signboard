#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "Signboard/RHI/detail/standardQueues.h"

namespace rhi {

	class creDevice;

	class pmvCommandBuffer;
	class pmvSemaphore;
	class pmvFence;

	class pcdQueueSubmission {
	public:
		pcdQueueSubmission(const rhi::creDevice& device) noexcept;

		void reset() noexcept;

		pcdQueueSubmission& update_toWait_semaphores(const rhi::pmvSemaphore* pSemaphores, const VkPipelineStageFlags* pWaitStages, uint32_t count) noexcept;
		pcdQueueSubmission& update_toSignal_semaphores(const rhi::pmvSemaphore* pSemaphores, uint32_t count) noexcept;
		pcdQueueSubmission& set_toTrigger_fence(const rhi::pmvFence& toTrigger_fence) noexcept;
		pcdQueueSubmission& update_toSubmit_cmd(const rhi::pmvCommandBuffer* cmdBuffers, uint32_t cmd_count) noexcept;

		VkResult submit_graphics_cmd();
		VkResult submit_compute_cmd();
		VkResult submit_transfer_cmd();
		VkResult submit_present_cmd();

	private:
		const standardQueues& r_queues;

		std::vector<VkSemaphore> toWait_semaphores;
		std::vector<VkPipelineStageFlags> toWait_stages;
		std::vector<VkSemaphore> toSignal_semaphores;

		VkFence toTrigger_fence;

		std::vector<VkCommandBuffer> toSubmit_cmd;

		VkSubmitInfo info{};
	};

}