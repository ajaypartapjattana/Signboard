#pragma once

namespace rhi::procedure {
	class commandBuffer_allocator;
}

#include <vulkan/vulkan.h>

namespace rhi::primitive {

	struct commandBuffer_vkAccess;

	class commandBuffer {
	public:
		commandBuffer() noexcept;

		commandBuffer(const commandBuffer&) = delete;
		commandBuffer& operator=(const commandBuffer&) = delete;

		commandBuffer(commandBuffer&&) noexcept;
		commandBuffer& operator=(commandBuffer&&) noexcept;

		~commandBuffer() noexcept;

		VkCommandBuffer native_commandBuffer() const noexcept;

	private:
		friend class rhi::procedure::commandBuffer_allocator;
		friend struct commandBuffer_vkAccess;

		VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;

		VkCommandPool m_commandPool = VK_NULL_HANDLE;

	};

}