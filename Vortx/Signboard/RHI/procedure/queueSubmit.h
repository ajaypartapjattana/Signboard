#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

#include "Signboard/Core/core.h"
#include "Signboard/RHI/detail/standardQueues.h"

namespace rhi {

	class creDevice;

	class pmvCommandBuffer;
	class pmvSemaphorePool;
	class pmvFence;

	class pcdQueueSubmit {
	public:
		pcdQueueSubmit(VkSubmitInfo* pSubmitInfo = nullptr) noexcept;

		void waitSemaphores(sgb::span<VkSemaphore> semaphores, sgb::span<const VkPipelineStageFlags> stages) noexcept;
		void signalSemaphores(sgb::span<VkSemaphore> semaphores) noexcept;
		void target_commandBuffers(sgb::span<const pmvCommandBuffer> commandBuffers) noexcept;

		VkResult submit(VkQueue queue, const pmvFence* fence) const noexcept;

		void preset(VkSubmitInfo* pSubmitInfo) noexcept;
		void reset() noexcept;

	private:
		VkSubmitInfo* allot_basic(VkSubmitInfo* pSubmitInfo) noexcept;

	private:
		std::vector<VkCommandBuffer> buffers;

		std::unique_ptr<VkSubmitInfo> m_ownedInfo;
		VkSubmitInfo* pInfo;

	};

}