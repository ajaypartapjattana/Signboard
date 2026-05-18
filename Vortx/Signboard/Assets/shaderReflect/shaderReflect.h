#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <unordered_map>

#include <external/SPIRV_cross/spirv_cross.hpp>

constexpr bool config_Validate =
#ifdef _VALIDATE
	true;
#else
	false;
#endif

struct VertexLayoutDesc {
	std::vector<VkVertexInputAttributeDescription> attributes;
	std::vector<VkVertexInputBindingDescription> bindings;
};

struct BindingKey {
	uint32_t set;
	uint32_t binding;

	bool operator==(const BindingKey& other) const noexcept {
		return set == other.set && binding == other.binding;
	}
};
struct BindingKeyHash {
	size_t operator()(const BindingKey& k) const noexcept {
		return (size_t(k.set) << 32) | k.binding;
	}
};
using BindingMap = std::unordered_map<BindingKey, VkDescriptorSetLayoutBinding, BindingKeyHash>;

class ShaderReflect {
public:
	ShaderReflect(const std::vector<uint32_t>&);

	ShaderReflect(const ShaderReflect&) = delete;
    ShaderReflect& operator=(const ShaderReflect&) = delete;

    ShaderReflect(ShaderReflect&&) = delete;
    ShaderReflect& operator=(ShaderReflect&&) = delete;

	VertexLayoutDesc reflectVertexLayout() const noexcept(!config_Validate);
	BindingMap reflectDescriptorBindings(VkShaderStageFlags stage) const noexcept(!config_Validate);
	std::vector<VkPushConstantRange> reflectPushConstants(VkShaderStageFlags stage) const noexcept(!config_Validate);

private:
	std::unique_ptr<spirv_cross::Compiler> createCompiler(const std::vector<uint32_t>& bin) const noexcept;

private:
	std::unique_ptr<spirv_cross::Compiler> m_compiler;

};
