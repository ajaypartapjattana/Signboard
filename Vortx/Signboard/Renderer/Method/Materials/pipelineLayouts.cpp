#include "pipelineLayouts.h"

#include <map>

namespace rndr {

	pipelineLayouts::pipelineLayouts(const rhi::creDevice& device, sgb::vltWrite<rhi::pmvPipelineLayout> pipelineLayoutWrite) noexcept
		:
		_wrt(pipelineLayoutWrite),
		m_create(device)
	{

	}

	uint32_t pipelineLayouts::create(sgb::span<const rhi::pmvDescriptorSetLayout*> descriptorLayouts, sgb::span<const VkPushConstantRange> pushConstantRanges) {
		m_create.target_descriptorSetLayouts(descriptorLayouts);
		m_create.target_pushConstantRanges(pushConstantRanges);

		auto builder = [&](rhi::pmvPipelineLayout* pl) {
			m_create.publish(*pl);
		};

		return _wrt.construct(builder);
	}

}