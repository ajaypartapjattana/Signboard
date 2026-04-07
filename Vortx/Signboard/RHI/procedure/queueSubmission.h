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

		pcdQueueSubmission& add_wait(const rhi::pmvSemaphore& sem, VkPipelineStageFlags stage) noexcept;
		pcdQueueSubmission& add_signal(const rhi::pmvSemaphore& sem) noexcept;
		pcdQueueSubmission& set_toTrigger_fence(const rhi::pmvFence& fence) noexcept;
		pcdQueueSubmission& add_cmd(const rhi::pmvCommandBuffer& cmd) noexcept;

		VkResult submit_graphics();
		VkResult submit_compute();
		VkResult submit_transfer();
		VkResult submit_present();

	private:
		void _prepare_submit() noexcept;

	private:
		const standardQueues& r_queues;

		std::vector<VkSemaphore> waitSemaphores;
		std::vector<VkPipelineStageFlags> waitStages;
		std::vector<VkSemaphore> signalSemaphores;
		std::vector<VkCommandBuffer> cmdBuffers;

		VkFence toTrigger_fence;

		VkSubmitInfo info{};
	};

}