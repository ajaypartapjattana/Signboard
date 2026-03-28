#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	enum class ShaderValidationErrorType {
		MISSING_ATTRIBUTE,
		FORMAT_MISMATCH,
		EXTRA_ATTRIBUTE
	};

	struct ValidationError {
		ShaderValidationErrorType type;
		uint32_t location;
		VkFormat expected;
		VkFormat actual;
	};

	struct ValidationResult {
		VkResult result;
		std::vector<ValidationError> errors;
	};

	class creDevice;

	class pmvShader;
	class pmvVertexLayout;

	class pcdShaderWrapper {
	public:
		pcdShaderWrapper(const creDevice& device) noexcept;

		pcdShaderWrapper& addBinary(const char* data, size_t size);
		
		VkResult warpShaderCode(pmvShader& targetShader) const;
		ValidationResult validateVertexLayout(const pmvVertexLayout& layout) const;

	private:
		VkDevice m_Device;

		struct {
			const char* data = nullptr;
			size_t size = 0;
		} m_binary;

	};

}