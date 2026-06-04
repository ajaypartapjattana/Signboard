#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

#include "Signboard/Core/core.h"

namespace rhi {

	class device;

	class pmvPipelineLayout;

	class pmvDescriptorSetLayout;
	class pmvPushConstantRange;

	class pcdPipelineLayoutCreate {
	public:
		pcdPipelineLayoutCreate(const device& device, VkPipelineLayoutCreateInfo* pCreateInfo = nullptr);

		void target_descriptorSetLayouts(sgb::span<const pmvDescriptorSetLayout*> layouts) noexcept;
		void target_pushConstantRanges(sgb::span<const VkPushConstantRange> pushConstantRanges) noexcept;

		VkResult publish(pmvPipelineLayout& pipelineLayout) const noexcept;

		void preset(VkPipelineLayoutCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkPipelineLayoutCreateInfo* allot_basic(VkPipelineLayoutCreateInfo* pCreateInfo) noexcept;

	private:
		VkDevice r_device;

		std::vector<VkDescriptorSetLayout> m_setLayouts;
		
		std::unique_ptr<VkPipelineLayoutCreateInfo> m_ownedInfo;
		VkPipelineLayoutCreateInfo* pInfo;

	};

}