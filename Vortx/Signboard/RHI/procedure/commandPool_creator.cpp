#include "commandPool_creator.h"

#include "Signboard/RHI/core/commandPool.h"
#include "Signboard/RHI/core/device_vk.h"

#include <algorithm>

namespace rhi::procedure {

	commandPool_creator::commandPool_creator(const rhi::core::device& device)
		: m_device(device)
	{

	}

	void commandPool_creator::collect_poolRequirements() noexcept {
		if (!m_requirements.empty())
			return;

		for (const auto& entry : rhi::core::device_vkAccess::get_queues(m_device)) {
			auto requirement_it = std::find_if(
				m_requirements.begin(),
				m_requirements.end(),
				[&entry](const pool_requirement& requirement) {
					return requirement.family == entry.family;
				}
			);

			if (requirement_it == m_requirements.end()) {
				m_requirements.push_back({ entry.family, entry.capabilities, entry.present_supported });
				continue;
			}

			requirement_it->capabilities |= entry.capabilities;
			requirement_it->present_supported = requirement_it->present_supported || entry.present_supported;
		}
	}

	uint32_t commandPool_creator::get_requiredPoolCount() noexcept {
		collect_poolRequirements();
		return static_cast<uint32_t>(m_requirements.size());
	}

	const std::vector<commandPool_creator::pool_requirement>& commandPool_creator::get_poolRequirements() noexcept {
		collect_poolRequirements();
		return m_requirements;
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

		for (uint32_t i = 0; i < m_requirements.size(); ++i) {
			poolInfo.queueFamilyIndex = m_requirements[i].family;

			VkCommandPool vk_commandPool = VK_NULL_HANDLE;
			VkResult result = vkCreateCommandPool(a_device, &poolInfo, nullptr, &vk_commandPool);
			if (result != VK_SUCCESS)
				return result;

			pools[i].m_commandPool = vk_commandPool;
			pools[i].m_queueFamilyIndex = m_requirements[i].family;
			pools[i].m_device = a_device;
		}

		return VK_SUCCESS;
	}

}
