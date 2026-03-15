#pragma once

namespace rhi::core {
	class device;
}

namespace rhi::primitive {
	class commandBuffer;
}

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::procedure {

	class queue_submission {
	public:
		queue_submission(const rhi::core::device& device) noexcept;



		VkResult submit(std::vector<rhi::primitive::commandBuffer>& cmdBuffers);

	private:
		const rhi::core::device& r_device;

		std::vector<VkSemaphore> toWait_semaphores;
		std::vector<VkPipelineStageFlags> toWait_stage;

		std::vector<VkSemaphore> toSignal_semaphores;
	};

}