#include "Signboard/RHI/Internal/rhi_pAccess.h"

#include <array>

namespace rhi {

	pcdCommandPoolCreate::pcdCommandPoolCreate(const creDevice& device, VkCommandPoolCreateInfo* pCreateInfo) noexcept
		: 
		r_device(_pAccess::extract(device)),
		families(_pAccess::queueFamilies(device)),
		pInfo(allot_basic(pCreateInfo))
	{

	}

	VkCommandPoolCreateInfo* pcdCommandPoolCreate::allot_basic(VkCommandPoolCreateInfo* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<VkCommandPoolCreateInfo>();

		VkCommandPoolCreateInfo* _info = m_ownedInfo.get();
		_info->sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

		return _info;
	}

	void pcdCommandPoolCreate::flag(VkCommandPoolCreateFlags flags) noexcept {
		pInfo->flags = flags;
	}

	VkResult pcdCommandPoolCreate::create(stdCommandPools& pools) {
		std::array<uint32_t, 4> l_families = { families.graphics, families.compute, families.transfer, families.present };

		std::vector<VkCommandPool> _pools(4);
		for (uint32_t i = 0; i < 4; ++i) {
			pInfo->queueFamilyIndex = l_families[i];
			VkResult result = vkCreateCommandPool(r_device, pInfo, nullptr, &_pools[i]);
			if (result != VK_SUCCESS) {
				for (uint32_t j = 0; j < i; ++j)
					vkDestroyCommandPool(r_device, _pools[j], nullptr);
				return result;
			}
		}

		pools.graphicsPool.m_commandPool = _pools[0];
		pools.graphicsPool.family = families.graphics;
		pools.graphicsPool.r_device = r_device;

		pools.computePool.m_commandPool = _pools[1];
		pools.computePool.family = families.compute;
		pools.computePool.r_device = r_device;

		pools.transferPool.m_commandPool = _pools[2];
		pools.transferPool.family = families.transfer;
		pools.transferPool.r_device = r_device;

		pools.presentPool.m_commandPool = _pools[3];
		pools.presentPool.family = families.present;
		pools.presentPool.r_device = r_device;

		return VK_SUCCESS;
	}

}
