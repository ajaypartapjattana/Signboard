#pragma once

namespace rhi::core {
	class device;
}

namespace rhi::primitive {
	class commandPool;
	class commandBuffer;
}

#include <vulkan/vulkan.h>

namespace rhi::procedure {

	class commandBuffer_allocator {
	public:
		commandBuffer_allocator(const rhi::core::device& device) noexcept;

		VkResult allocate(const rhi::primitive::commandPool& commandPool, rhi::primitive::commandBuffer* target_cb, uint32_t count) const noexcept;

	private:
		VkDevice m_device;

	};

}