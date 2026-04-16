#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	class creDevice;

	class pmvShader;

	class pcdShaderWrapper {
	public:
		pcdShaderWrapper(const creDevice& device) noexcept;

		pcdShaderWrapper& setBinary(const std::vector<uint32_t>& data);
		
		VkResult wrapShaderCode(pmvShader& targetShader) const;

	private:
		VkDevice m_Device;

		const std::vector<uint32_t>* m_binary;

	};

}