#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	class creDevice;

	class pmvPipelineLayout;

	class pmvDescriptorLayout;
	class pmvPushConstantRange;

	class pcdPipelineLayoutCreate {
	public:
		pcdPipelineLayoutCreate(const rhi::creDevice& device);

		pcdPipelineLayoutCreate& add_setLayout(const rhi::pmvDescriptorLayout& descriptorLayout);
		pcdPipelineLayoutCreate& add_pushConstantRange(const rhi::pmvPushConstantRange& pushConstantRange);

		VkResult build(rhi::pmvPipelineLayout& target_layout);

	private:
		VkDevice _dvc;

		std::vector<VkDescriptorSetLayout> m_setLayouts;
		std::vector<VkPushConstantRange> m_pushConstantRanges;

	};

}