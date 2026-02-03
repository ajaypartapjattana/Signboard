#include "commandPool_creator.h"

#include "Signboard/RHI/core/commandPool.h"
#include "Signboard/RHI/core/device_vk.h"

#include <stdexcept>

namespace rhi::procedure {

	commandPool_creator::commandPool_creator(const rhi::core::device& device)
		: m_device(device)
	{

	}

	uint32_t commandPool_creator::get_requiredPoolCount() noexcept {
		for (const auto& entry : rhi::core::device_vkAccess::get_queues(m_device)) {
			if (std::find(m_families.begin(), m_families.end(), entry.family) == m_families.end())
				m_families.push_back(entry.family);
		}

		return static_cast<uint32_t>(m_families.size());
	}

	VkResult commandPool_creator::create(rhi::core::commandPool* pools, uint32_t poolCount) {
		if (!pools)
			return VK_ERROR_INITIALIZATION_FAILED;

		if (poolCount < m_families.size())
			return VK_INCOMPLETE;

		VkDevice a_device = rhi::core::device_vkAccess::get(m_device);

		std::vector<rhi::core::commandPool> l_pools;
		l_pools.reserve(m_families.size());
		
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

		for (uint32_t i = 0; i < m_families.size(); ++i) {
			poolInfo.queueFamilyIndex = m_families[i];

			VkCommandPool vk_commandPool = VK_NULL_HANDLE;
			if (vkCreateCommandPool(a_device, &poolInfo, nullptr, &vk_commandPool) != VK_SUCCESS)
				throw std::runtime_error("FAILURE: commandPool_creation!");

			pools[i].m_commandPool = vk_commandPool;
			pools[i].m_queueFamilyIndex = m_families[i];
			pools[i].m_device = a_device;
		}

		return VK_SUCCESS;
	}

}