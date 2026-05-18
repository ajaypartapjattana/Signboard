#pragma once

#include <vulkan/vulkan.h>

namespace rhi {
	
	struct _pAccess;

	class pcdShaderModuleCreate;

	class pmvShader {
	public:
		pmvShader() noexcept;

		pmvShader(const pmvShader&) noexcept;
		pmvShader& operator=(const pmvShader&) noexcept;

		pmvShader(pmvShader&&) noexcept;
		pmvShader& operator=(pmvShader&&) noexcept;

		~pmvShader() noexcept;

		void reset() noexcept;

	private:
		friend class pcdShaderModuleCreate;
		friend struct _pAccess;

		VkShaderModule m_shader;
		VkShaderStageFlagBits stage;

		VkDevice r_device;

	};

}