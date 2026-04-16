#include "shaderReflect.h"

ShaderReflect::ShaderReflect(const std::vector<uint32_t>& bin) 
	:
	m_bin(bin)
{

}

std::vector<VertexAttribute> ShaderReflect::reflectVertexLayout() const {
	std::vector<VertexAttribute> result;

	std::unique_ptr<spirv_cross::Compiler> _compile = std::make_unique<spirv_cross::Compiler>(m_bin);
	spirv_cross::ShaderResources _res = _compile->get_shader_resources();

	for (const spirv_cross::Resource& input : _res.stage_inputs) {
		uint32_t location = _compile->get_decoration(input.id, spv::DecorationLocation);
		const auto& type = _compile->get_type(input.type_id);

		result.push_back({ location, mapTypeToFormat(type) });
	}

	std::sort(result.begin(), result.end(),	[](const VertexAttribute& a, const VertexAttribute& b) {
			return a.location < b.location;
		});

	return result;
}

std::vector<DescriptorBinding> ShaderReflect::reflectDescriptorBindings(VkShaderStageFlags stage) const {
	std::vector<DescriptorBinding> result;

	std::unique_ptr<spirv_cross::Compiler> _compile = std::make_unique<spirv_cross::Compiler>(m_bin);

	auto process = [&](const spirv_cross::SmallVector<spirv_cross::Resource>& resources, VkDescriptorType type) {
		for (const spirv_cross::Resource& res : resources) {
			uint32_t set = _compile->get_decoration(res.id, spv::DecorationDescriptorSet);
			uint32_t binding = _compile->get_decoration(res.id, spv::DecorationBinding);
			const spirv_cross::SPIRType& spirType = _compile->get_type(res.type_id);
			uint32_t count = 1;

			if (!spirType.array.empty()) {
				if (spirType.array_size_literal[0])
					count = spirType.array[0];
				else
					count = 1;
			}

			result.push_back({ set, binding, type, count, stage });
		}
	};

	spirv_cross::ShaderResources _res = _compile->get_shader_resources();

	process(_res.uniform_buffers, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	process(_res.storage_buffers, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
	process(_res.sampled_images, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	process(_res.separate_images, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
	process(_res.separate_samplers, VK_DESCRIPTOR_TYPE_SAMPLER);
	process(_res.storage_images, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);

	return result;
}

std::vector<PushConstantRangeInfo> ShaderReflect::reflectPushConstants(VkShaderStageFlags stage) const {
	std::vector<PushConstantRangeInfo> result;
	
	std::unique_ptr<spirv_cross::Compiler> _compile = std::make_unique<spirv_cross::Compiler>(m_bin);
	spirv_cross::ShaderResources _res = _compile->get_shader_resources();

	for (const spirv_cross::Resource& pc : _res.push_constant_buffers) {
		const spirv_cross::SPIRType& type = _compile->get_type(pc.base_type_id);
		uint32_t size = static_cast<uint32_t>(_compile->get_declared_struct_size(type));
		
		result.push_back({ size, 0, stage });
	}

	return result;
}

VkFormat ShaderReflect::mapTypeToFormat(const spirv_cross::SPIRType& type) {
	if (type.basetype == spirv_cross::SPIRType::Float) {
		switch (type.vecsize) {
		case 1: return VK_FORMAT_R32_SFLOAT;
		case 2: return VK_FORMAT_R32G32_SFLOAT;
		case 3: return VK_FORMAT_R32G32B32_SFLOAT;
		case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
		}
	}

	if (type.basetype == spirv_cross::SPIRType::Int) {
		switch (type.vecsize) {
		case 1: return VK_FORMAT_R32_SINT;
		case 2: return VK_FORMAT_R32G32_SINT;
		case 3: return VK_FORMAT_R32G32B32_SINT;
		case 4: return VK_FORMAT_R32G32B32A32_SINT;
		}
	}

	if (type.basetype == spirv_cross::SPIRType::UInt) {
		switch (type.vecsize) {
		case 1: return VK_FORMAT_R32_UINT;
		case 2: return VK_FORMAT_R32G32_UINT;
		case 3: return VK_FORMAT_R32G32B32_UINT;
		case 4: return VK_FORMAT_R32G32B32A32_UINT;
		}
	}

	return VK_FORMAT_UNDEFINED;
}