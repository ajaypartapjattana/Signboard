#pragma once

namespace rhi::core {
	class device;
}

namespace rhi::primitive {
	class commandBuffer;
	class semaphore;
	class fence;
}

#include "Signboard/RHI/core/device.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::procedure {

	class queue_submission {
	public:
		queue_submission(const rhi::core::device& device) noexcept;

		void reset() noexcept;

		queue_submission& update_toWait_semaphores(const rhi::primitive::semaphore* pSemaphores, const VkPipelineStageFlags* pWaitStages, uint32_t count) noexcept;
		queue_submission& update_toSignal_semaphores(const rhi::primitive::semaphore* pSemaphores, uint32_t count) noexcept;
		queue_submission& set_toTrigger_fence(const rhi::primitive::fence& toTrigger_fence) noexcept;
		queue_submission& update_toSubmit_cmd(const rhi::primitive::commandBuffer* cmdBuffers, uint32_t cmd_count) noexcept;

		VkResult submit_graphics_cmd();
		VkResult submit_compute_cmd();
		VkResult submit_transfer_cmd();
		VkResult submit_present_cmd();

	private:
		const rhi::core::device::queues& r_queues;

		std::vector<VkSemaphore> toWait_semaphores;
		std::vector<VkPipelineStageFlags> toWait_stages;
		std::vector<VkSemaphore> toSignal_semaphores;

		VkFence toTrigger_fence;

		std::vector<VkCommandBuffer> toSubmit_cmd;

		VkSubmitInfo info{};
	};

}