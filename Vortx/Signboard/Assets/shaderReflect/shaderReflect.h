#pragma once

#include <vulkan/vulkan.h>
#include <spirv_cross/spirv_cross.hpp>
#include <memory>

struct VertexAttribute {
	uint32_t location;
	VkFormat format;
};

struct DescriptorBinding {
	uint32_t set;
	uint32_t binding;
	VkDescriptorType type;
	uint32_t count;
	VkShaderStageFlags stage;
};

struct PushConstantRangeInfo {
	uint32_t size;
	uint32_t offset;
	VkShaderStageFlags stage;
};

class ShaderReflect {
public:
	ShaderReflect(const std::vector<uint32_t>&);

	ShaderReflect(const ShaderReflect&) = delete;
    ShaderReflect& operator=(const ShaderReflect&) = delete;

    ShaderReflect(ShaderReflect&&) = delete;
    ShaderReflect& operator=(ShaderReflect&&) = delete;

	std::vector<VertexAttribute> reflectVertexLayout() const;
	std::vector<DescriptorBinding> reflectDescriptorBindings(VkShaderStageFlags stage) const;
	std::vector<PushConstantRangeInfo> reflectPushConstants(VkShaderStageFlags stage) const;

private:
	static VkFormat mapTypeToFormat(const spirv_cross::SPIRType& type);

private:
	const std::vector<uint32_t>& m_bin;

};
