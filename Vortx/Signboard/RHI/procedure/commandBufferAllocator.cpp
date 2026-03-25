#include "commandBufferAllocator.h"

#include "Signboard/RHI/primitive/commandBuffer.h"

#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/commandPool_pAccess.h"

#include <stdexcept>

namespace rhi {

	pcdCommandBufferAllocator::pcdCommandBufferAllocator(const rhi::creDevice& device) noexcept
		: 
		m_device(rhi::access::device_pAccess::get(device))
	{
	
	}

	VkResult pcdCommandBufferAllocator::allocate(const rhi::pmvCommandPool& commandPool, rhi::pmvCommandBuffer* target_cb, uint32_t count) const noexcept {
		if (count == 0 || !target_cb)
			return VK_INCOMPLETE;

		VkCommandPool a_commandPool = rhi::access::commandPool_pAccess::get(commandPool);
		
		std::vector<VkCommandBuffer> vk_buffers(count);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = a_commandPool;
		allocInfo.commandBufferCount = count;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		
		VkResult result = vkAllocateCommandBuffers(m_device, &allocInfo, vk_buffers.data());
		if (result != VK_SUCCESS)
			return result;

		for (uint32_t i = 0; i < count; ++i) {
			target_cb[i].m_commandBuffer = vk_buffers[i];
			target_cb[i].m_commandPool = a_commandPool;
		}

		return VK_SUCCESS;
	}

}