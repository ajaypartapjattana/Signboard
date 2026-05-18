#pragma once

#include <memory>

#include "Signboard/RHI/detail/standardCommandPools.h"

namespace rhi {

	class creDevice;

	class pcdCommandPoolCreate {
	public:
		pcdCommandPoolCreate(const creDevice& device, VkCommandPoolCreateInfo* pCreateInfo = nullptr) noexcept;

		void flag(VkCommandPoolCreateFlags flags) noexcept;

		VkResult create(stdCommandPools& commandPools);

	private:
		VkCommandPoolCreateInfo* allot_basic(VkCommandPoolCreateInfo* pCreateInfo) noexcept;

	private:
		const VkDevice r_device;
		const stdQueueFamilies& families;

		std::unique_ptr<VkCommandPoolCreateInfo> m_ownedInfo;
		VkCommandPoolCreateInfo* pInfo;

	};

}
