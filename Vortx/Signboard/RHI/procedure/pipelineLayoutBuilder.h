#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	class creDevice;

	class pmvPipelineLayout;

	class pmvDescriptorSetLayout;
	class pmvPushConstantRange;

	class pcdPipelineLayoutCreate {
	public:
		pcdPipelineLayoutCreate(const rhi::creDevice& device, VkPipelineLayoutCreateInfo* pCreateInfo = nullptr);

		pcdPipelineLayoutCreate& push_descriptorSetLayouts(const rhi::pmvDescriptorSetLayout& descriptorLayout);
		pcdPipelineLayoutCreate& push_descriptorSetLayouts(const std::vector<rhi::pmvDescriptorSetLayout>& descriptorLayouts);
		pcdPipelineLayoutCreate& target_pushConstantRanges(const std::vector<VkPushConstantRange>& pushConstantRanges);

		VkResult publish(rhi::pmvPipelineLayout& pipelineLayout);

		void preset(VkPipelineLayoutCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkPipelineLayoutCreateInfo fetch_basic(VkPipelineLayoutCreateInfo* pCreateInfo) const noexcept;

	private:
		VkDevice r_device;

		std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
		
		VkPipelineLayoutCreateInfo _info;

	};

}