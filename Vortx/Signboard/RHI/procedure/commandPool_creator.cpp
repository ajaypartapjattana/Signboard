#include "commandPool_creator.h"

#include "Signboard/RHI/core/device_vk.h"

#include <array>

namespace rhi::procedure {

	commandPool_creator::commandPool_creator(const rhi::core::device& device)
		: m_device(device)
	{

	}

	VkResult commandPool_creator::create(std_commandPools& pools) {
		VkDevice a_device = rhi::core::device_vkAccess::get(m_device);
		const auto& families = rhi::core::device_vkAccess::get_queueFamilies(m_device);

		std::array<uint32_t, 4> l_families = { families.graphics, families.compute, families.transfer, families.present };
		std::array<VkCommandPool, 4> l_pools{};

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

		for (uint32_t i = 0; i < 4; ++i) {
			poolInfo.queueFamilyIndex = l_families[i];
			VkResult result = vkCreateCommandPool(a_device, &poolInfo, nullptr, &l_pools[i]);
			if (result != VK_SUCCESS) {
				for (uint32_t j = 0; j < i; ++j)
					vkDestroyCommandPool(a_device, l_pools[j], nullptr);
				return result;
			}
		}
		pools.graphicsPool.m_commandPool = l_pools[0];
		pools.graphicsPool.m_queueFamilyIndex = families.graphics;
		pools.graphicsPool.m_device = a_device;

		pools.computePool.m_commandPool = l_pools[1];
		pools.computePool.m_queueFamilyIndex = families.compute;
		pools.computePool.m_device = a_device;

		pools.transferPool.m_commandPool = l_pools[2];
		pools.transferPool.m_queueFamilyIndex = families.transfer;
		pools.transferPool.m_device = a_device;

		pools.presentPool.m_commandPool = l_pools[3];
		pools.presentPool.m_queueFamilyIndex = families.present;
		pools.presentPool.m_device = a_device;

		return VK_SUCCESS;
	}

}
