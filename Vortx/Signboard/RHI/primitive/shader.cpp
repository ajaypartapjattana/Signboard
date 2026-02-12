#include "shader.h"

namespace rhi::primitive {

	shader::shader() noexcept
		: m_shader(VK_NULL_HANDLE), m_device(VK_NULL_HANDLE)
	{
	
	}

	shader::shader(shader&& other) noexcept {
		m_shader = other.m_shader;
		m_device = other.m_device;

		other.m_shader = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
	}

	shader& shader::operator=(shader&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_shader)
			vkDestroyShaderModule(m_device, m_shader, nullptr);

		m_shader = other.m_shader;
		m_device = other.m_device;

		other.m_shader = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;

		return *this;
	}

	shader::~shader() noexcept {
		if (m_shader)
			vkDestroyShaderModule(m_device, m_shader, nullptr);
	}

	VkShaderModule shader::native_shader() const noexcept {
		return m_shader;
	}

}