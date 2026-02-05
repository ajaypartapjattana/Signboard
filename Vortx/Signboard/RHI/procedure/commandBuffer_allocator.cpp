#include "commandBuffer_allocator.h"

#include "Signboard/RHI/primitive/commandBuffer.h"

#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/core/commandPool_vk.h"

#include <stdexcept>

namespace rhi::procedure {

	commandBuffer_allocator::commandBuffer_allocator(const rhi::core::device& device) noexcept
		: m_device(rhi::core::device_vkAccess::get(device))
	{
	
	}

	void commandBuffer_allocator::allocate(const rhi::core::commandPool& commandPool, rhi::primitive::commandBuffer* target_cb, uint32_t count) {
		if (count == 0 || !target_cb)
			return;

		VkCommandPool a_commandPool = rhi::core::commandPool_vkAccess::get(commandPool);
		
		std::vector<VkCommandBuffer> vk_buffers(count);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = a_commandPool;
		allocInfo.commandBufferCount = count;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		
		if (vkAllocateCommandBuffers(m_device, &allocInfo, vk_buffers.data()) != VK_SUCCESS)
			throw std::runtime_error("FAILURE: commandBuffer_allocation!");

		for (uint32_t i = 0; i < count; ++i) {
			target_cb[i].m_commandBuffer = vk_buffers[i];
			target_cb[i].m_commandPool = a_commandPool;
		}
	}

}