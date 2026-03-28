#include "shaderWrapper.h"

#include <spirv_cross/spirv_cross.hpp>

#include "Signboard/RHI/primitive/shader.h"
#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/vertexLayout_pAccess.h"

namespace rhi {

	pcdShaderWrapper::pcdShaderWrapper(const rhi::creDevice& device) noexcept
		: 
		m_Device(rhi::access::device_pAccess::get(device))
	{

	}

	pcdShaderWrapper& pcdShaderWrapper::addBinary(const char* data, size_t size) {
		m_binary = { data, size };

		return *this;
	}

	VkResult pcdShaderWrapper::warpShaderCode(pmvShader& tgtShader) const {
		if (!m_binary.data || !*m_binary.data)
			return VK_INCOMPLETE;

		if (m_binary.size % 4 != 0)
			return VK_ERROR_INVALID_SHADER_NV;

		VkShaderModuleCreateInfo shaderInfo{};
		shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderInfo.pCode = reinterpret_cast<const uint32_t*>(m_binary.data);
		shaderInfo.codeSize = m_binary.size;

		VkResult result = vkCreateShaderModule(m_Device, &shaderInfo, nullptr, &tgtShader.m_shader);
		if (result != VK_SUCCESS)
			return result;

		tgtShader.m_device = m_Device;

		return VK_SUCCESS;

	}

	struct ShaderAttribute {
		uint32_t location;
		VkFormat format;
	};

	static VkFormat mapTypeToFormat(const spirv_cross::SPIRType& type) {
		using namespace spirv_cross;

		if (type.basetype == SPIRType::Float) {
			switch (type.vecsize) {
			case 1: return VK_FORMAT_R32_SFLOAT;
			case 2: return VK_FORMAT_R32G32_SFLOAT;
			case 3: return VK_FORMAT_R32G32B32_SFLOAT;
			case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
			}
		}

		if (type.basetype == SPIRType::Int) {
			switch (type.vecsize) {
			case 1: return VK_FORMAT_R32_SINT;
			case 2: return VK_FORMAT_R32G32_SINT;
			case 3: return VK_FORMAT_R32G32B32_SINT;
			case 4: return VK_FORMAT_R32G32B32A32_SINT;
			}
		}

		if (type.basetype == SPIRType::UInt) {
			switch (type.vecsize) {
			case 1: return VK_FORMAT_R32_UINT;
			case 2: return VK_FORMAT_R32G32_UINT;
			case 3: return VK_FORMAT_R32G32B32_UINT;
			case 4: return VK_FORMAT_R32G32B32A32_UINT;
			}
		}

		throw std::runtime_error("LOGIC: unsupported_shader_input_type!");
	}

	static std::vector<ShaderAttribute> reflectVertexInputs(const std::vector<uint32_t>& spirv) {
		spirv_cross::Compiler compiler{ spirv };
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		std::vector<ShaderAttribute> result;

		for (const spirv_cross::Resource& input : resources.stage_inputs) {
			uint32_t location = compiler.get_decoration(input.id, spv::DecorationLocation);
			const auto& type = compiler.get_type(input.type_id);

			result.push_back({ location, mapTypeToFormat(type) });
		}

		return result;
	}

	static std::vector<uint32_t> toSpirvWords(const char* data, size_t size) {
		std::vector<uint32_t> words(size / 4);
		std::memcpy(words.data(), data, size);
		return words;
	}

	ValidationResult pcdShaderWrapper::validateVertexLayout(const pmvVertexLayout& layout) const {
		if (m_binary.size % 4 != 0)
			return { VK_ERROR_INVALID_SHADER_NV, {} };
		
		std::vector<uint32_t> spirv = toSpirvWords(m_binary.data, m_binary.size);
		
		std::vector<ShaderAttribute> shaderAttributes = reflectVertexInputs(spirv);
		std::vector<VkVertexInputAttributeDescription> vertexAttributes = rhi::access::vertexLayout_pAccess::getDescription(layout).attributes;

		ValidationResult result{};

		for (const ShaderAttribute& shaderAttribute : shaderAttributes) {
			bool found = false;

			for (const VkVertexInputAttributeDescription& vertexAttribute : vertexAttributes) {
				if (vertexAttribute.location == shaderAttribute.location) {
					found = true;

					if (vertexAttribute.format != shaderAttribute.format) {
						result.result = VK_INCOMPLETE;
						result.errors.push_back({
							ShaderValidationErrorType::FORMAT_MISMATCH,
							vertexAttribute.location,
							shaderAttribute.format,
							vertexAttribute.format
						});
					}
					break;
				}
			}
			if (!found) {
				result.result = VK_INCOMPLETE;
				result.errors.push_back({
					ShaderValidationErrorType::MISSING_ATTRIBUTE,
					shaderAttribute.location,
					shaderAttribute.format,
					VK_FORMAT_UNDEFINED
				});
			}
		}
		for (const VkVertexInputAttributeDescription& vertexAttribute : vertexAttributes) {
			bool used = false;

			for (const ShaderAttribute& shaderAttribute : shaderAttributes) {
				if (shaderAttribute.location == vertexAttribute.location) {
					used = true;
					break;
				}
			}
			if (!used) {
				result.result = VK_INCOMPLETE;
				result.errors.push_back({
					ShaderValidationErrorType::EXTRA_ATTRIBUTE,
					vertexAttribute.location,
					VK_FORMAT_UNDEFINED,
					vertexAttribute.format
				});
			}
		}

		return result;
	}	

}