#pragma once

#include <vulkan/vulkan.h>

namespace rhi::access {
	struct commandBuffer_pAccess;
}

namespace rhi {

	class pcdCommandBufferAllocator;
	class pcdQueueSubmission;

	class pmvCommandBuffer {
	public:
		pmvCommandBuffer() noexcept;

		pmvCommandBuffer(const pmvCommandBuffer&) = delete;
		pmvCommandBuffer& operator=(const pmvCommandBuffer&) = delete;

		pmvCommandBuffer(pmvCommandBuffer&&) noexcept;
		pmvCommandBuffer& operator=(pmvCommandBuffer&&) noexcept;

		~pmvCommandBuffer() noexcept;

	private:
		friend class pcdCommandBufferAllocator;
		friend class pcdQueueSubmission;
		friend struct rhi::access::commandBuffer_pAccess;

		VkCommandBuffer m_commandBuffer;
		VkCommandPool m_commandPool;

	};

}