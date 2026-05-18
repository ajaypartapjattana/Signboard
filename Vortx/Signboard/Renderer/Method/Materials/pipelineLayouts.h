#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Core/core.h"


namespace rndr {

	class pipelineLayouts {
	public:
		pipelineLayouts(const rhi::creDevice& device, sgb::vltWrite<rhi::pmvPipelineLayout> pipelineLayoutWrite) noexcept;

		uint32_t create(sgb::span<const rhi::pmvDescriptorSetLayout*> descriptorLayouts, sgb::span<const VkPushConstantRange> pushConstantRanges);


	private:
		sgb::vltWrite<rhi::pmvPipelineLayout> _wrt;

		rhi::pcdPipelineLayoutCreate m_create;

	};

}