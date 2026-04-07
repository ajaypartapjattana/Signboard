#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	class creDevice;

	class pmvPipelineLayout;

	class pmvDescriptorLayout;
	class pmvPushConstantRange;

	class pcdPipelineLayoutBuilder {
	public:
		pcdPipelineLayoutBuilder(const rhi::creDevice& device);

		pcdPipelineLayoutBuilder& add_setLayout(const rhi::pmvDescriptorLayout& descriptorLayout);
		pcdPipelineLayoutBuilder& add_pushConstantRange(const rhi::pmvPushConstantRange& pushConstantRange);

		VkResult build(rhi::pmvPipelineLayout& target_layout);

	private:
		VkDevice _dvc;

		std::vector<VkDescriptorSetLayout> m_setLayouts;
		std::vector<VkPushConstantRange> m_pushConstantRanges;

	};

}