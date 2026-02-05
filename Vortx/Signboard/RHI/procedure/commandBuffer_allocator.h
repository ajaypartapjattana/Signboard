#pragma once

namespace rhi::core {
	class commandPool;
	class device;
}

namespace rhi::primitive {
	class commandBuffer;
}

#include <vulkan/vulkan.h>

namespace rhi::procedure {

	class commandBuffer_allocator {
	public:
		commandBuffer_allocator(const rhi::core::device& device) noexcept;

		void allocate(const rhi::core::commandPool& commandPool, rhi::primitive::commandBuffer* target_cb, uint32_t count);

	private:
		VkDevice m_device;

	};

}