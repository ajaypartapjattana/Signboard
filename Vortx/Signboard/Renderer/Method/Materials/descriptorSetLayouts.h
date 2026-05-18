#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Core/core.h"

#include "detail/shaderBinary.h"

namespace rndr {

	class descriptorSetLayouts {
	public:
		descriptorSetLayouts(const rhi::creDevice& device, sgb::vltWrite<rhi::pmvDescriptorSetLayout> descriptorSetLayoutWrite) noexcept;

		uint32_t create(sgb::span<const VkDescriptorSetLayoutBinding> bindings);
		std::vector<uint32_t> create(sgb::span<const shaderBinary*> shaders);

	private:
		sgb::vltWrite<rhi::pmvDescriptorSetLayout> _wrt;

		rhi::pcdDescriptorSetLayoutCreate m_create;

	};

}