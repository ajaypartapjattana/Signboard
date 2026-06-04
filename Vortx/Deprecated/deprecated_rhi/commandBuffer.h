#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	struct _pAccess;

	class pcdCommandBufferAllocate;
	class pcdQueueSubmit;

	class pmvCommandBuffer {
	public:
		pmvCommandBuffer() noexcept;

		pmvCommandBuffer(const pmvCommandBuffer&) = delete;
		pmvCommandBuffer& operator=(const pmvCommandBuffer&) = delete;

		pmvCommandBuffer(pmvCommandBuffer&&) noexcept;
		pmvCommandBuffer& operator=(pmvCommandBuffer&&) noexcept;

		~pmvCommandBuffer() noexcept;

	private:
		friend class pcdCommandBufferAllocate;
		friend class pcdQueueSubmit;
		friend struct _pAccess;

		VkCommandBuffer m_commandBuffer;

		VkCommandPool r_commandPool;

	};

}