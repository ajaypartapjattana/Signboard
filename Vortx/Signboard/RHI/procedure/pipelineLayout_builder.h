#pragma once

namespace rhi::core {
	class device;
}

namespace rhi::primitive {
	class pipelineLayout;

	class descriptorLayout;
	class pushConstantRange;
}

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::procedure {

	class pipelineLayout_builder {
	public:
		pipelineLayout_builder(const rhi::core::device& device);

		pipelineLayout_builder& add_setLayout(const rhi::primitive::descriptorLayout& descriptorLayout);
		pipelineLayout_builder& add_pushConstantRange(const rhi::primitive::pushConstantRange& pushConstantRange);

		VkResult build(rhi::primitive::pipelineLayout& target_layout);

	private:
		VkDevice m_device;

		std::vector<VkDescriptorSetLayout> m_setLayouts;
		std::vector<VkPushConstantRange> m_pushConstantRanges;

	};

}