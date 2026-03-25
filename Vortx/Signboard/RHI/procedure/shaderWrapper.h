#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	class creDevice;

	class pmvShader;

	class pcdShaderWrapper {
	public:
		pcdShaderWrapper(const rhi::creDevice& device) noexcept;

		pcdShaderWrapper& set_target(rhi::pmvShader& targetShader) noexcept;
		VkResult warpShaderCode(const char* data, size_t size);

	private:
		VkDevice m_Device;
		rhi::pmvShader* m_target;

	};

}