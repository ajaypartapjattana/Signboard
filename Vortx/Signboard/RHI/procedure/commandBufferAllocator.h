#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	class creDevice;

	class pmvCommandPool;
	class pmvCommandBuffer;

	class pcdCommandBufferAllocator {
	public:
		pcdCommandBufferAllocator(const rhi::creDevice& device) noexcept;

		VkResult allocate(const rhi::pmvCommandPool& commandPool, rhi::pmvCommandBuffer* target_cb, uint32_t count) const noexcept;

	private:
		VkDevice m_device;

	};

}