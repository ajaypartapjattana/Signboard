#include "commandPool_creator.h"

#include "Signboard/RHI/core/commandPool.h"
#include "Signboard/RHI/core/device_vk.h"

#include <algorithm>

namespace rhi::procedure {

	commandPool_creator::commandPool_creator(const rhi::core::device& device)
		: m_device(device)
	{

	}

	const std::vector<commandPool_creator::pool_reqirement>& commandPool_creator::get_poolRequirements() noexcept {
		collect_poolRequirements();
		return m_reqirements;
	}

	uint32_t commandPool_creator::get_requiredPoolCount() noexcept {
		collect_poolRequirements();
		return static_cast<uint32_t>(m_reqirements.size());
	}

	void commandPool_creator::collect_poolRequirements() noexcept {
		if (m_reqirements.empty())
			return;

		for (const auto& entry : rhi::core::device_vkAccess::get_queues(m_device)) {
			auto requirement_it = std::find_if(m_reqirements.begin(), m_reqirements.end(), [&entry](const pool_reqirement& requirement) {return requirement.family == entry.family; });

			if (requirement_it == m_reqirements.end()) {
				m_reqirements.push_back({ entry.family, entry.capabilities, entry.present_supported });
				continue;
			}

			requirement_it->capabilities |= entry.capabilities;
			requirement_it->present_supported = requirement_it->present_supported || entry.present_supported;
		}
	}

	VkResult commandPool_creator::create(rhi::core::commandPool* pools, uint32_t poolCount) {
		collect_poolRequirements();

		if (poolCount < m_reqirements.size())
			return VK_INCOMPLETE;

		if (!pools)
			return poolCount == 0 ? VK_SUCCESS : VK_ERROR_INITIALIZATION_FAILED;

		VkDevice a_device = rhi::core::device_vkAccess::get(m_device);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

		for (uint32_t i = 0; i < m_reqirements.size(); ++i) {
			poolInfo.queueFamilyIndex = m_reqirements[i].family;

			VkCommandPool vk_commandPool = VK_NULL_HANDLE;

			VkResult result = vkCreateCommandPool(a_device, &poolInfo, nullptr, &vk_commandPool);

			if (result != VK_SUCCESS)
				return result;

			pools[i].m_commandPool = vk_commandPool;
			pools[i].m_queueFamilyIndex = m_reqirements[i].family;
			pools[i].m_device = a_device;
		}

		return VK_SUCCESS;
	}

}