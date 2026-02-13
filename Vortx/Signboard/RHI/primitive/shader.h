#pragma once

namespace rhi::procedure {
	class shader_wrapper;
}

#include <vulkan/vulkan.h>

namespace rhi::primitive {
	
	struct shader_vkAccess;

	class shader {
	public:
		shader() noexcept;

		shader(const shader&) = delete;
		shader& operator=(const shader&) = delete;

		shader(shader&&) noexcept;
		shader& operator=(shader&&) noexcept;

		~shader() noexcept;

		VkShaderModule native_shader() const noexcept;

	private:
		friend class rhi::procedure::shader_wrapper;
		friend struct shader_vkAccess;

		VkShaderModule m_shader = VK_NULL_HANDLE;

		VkDevice m_device = VK_NULL_HANDLE;

	};

}