#pragma once

#include <vulkan/vulkan.h>

namespace rhi::access {
	struct shader_pAccess;
}

namespace rhi {
	
	class pcdShaderWrapper;

	class pmvShader {
	public:
		pmvShader() noexcept;

		pmvShader(const pmvShader&) = delete;
		pmvShader& operator=(const pmvShader&) = delete;

		pmvShader(pmvShader&&) noexcept;
		pmvShader& operator=(pmvShader&&) noexcept;

		~pmvShader() noexcept;

	private:
		friend class pcdShaderWrapper;
		friend struct rhi::access::shader_pAccess;

		VkShaderModule m_shader;
		VkDevice _dvc;

	};

}