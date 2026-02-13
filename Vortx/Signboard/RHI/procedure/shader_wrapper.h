#pragma once

namespace rhi::core {
	class device;
}

namespace rhi::primitive {
	class shader;
}

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::procedure {

	class shader_wrapper {
	public:
		shader_wrapper(const rhi::core::device& device) noexcept;

		shader_wrapper& set_target(rhi::primitive::shader& targetShader);

		VkResult wrap_shaderCode(const std::vector<char>& data);

	private:
		VkDevice m_Device;

		rhi::primitive::shader* m_target;

	};

}