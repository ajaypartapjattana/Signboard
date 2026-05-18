#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

#include "Signboard/Core/core.h"

namespace rhi {

	class creDevice;

	class pmvDescriptorSetLayout;

	class pcdDescriptorSetLayoutCreate {
	public:
		pcdDescriptorSetLayoutCreate(const creDevice& device, VkDescriptorSetLayoutCreateInfo* pCreateInfo = nullptr) noexcept;

		void target_bindings(sgb::span<const VkDescriptorSetLayoutBinding> bindings);

		VkResult publish(pmvDescriptorSetLayout& descriptorLayout) const noexcept;

		void preset(VkDescriptorSetLayoutCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkDescriptorSetLayoutCreateInfo* allot_basic(VkDescriptorSetLayoutCreateInfo* pCreateInfo) noexcept;

	private:
		const VkDevice r_device;

		std::unique_ptr<VkDescriptorSetLayoutCreateInfo> m_ownedInfo;
		VkDescriptorSetLayoutCreateInfo* pInfo;

	};

}