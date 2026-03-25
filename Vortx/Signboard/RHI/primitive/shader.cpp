#include "shader.h"

namespace rhi {

	pmvShader::pmvShader() noexcept
		: 
		m_shader(VK_NULL_HANDLE), 
		m_device(VK_NULL_HANDLE)
	{
	
	}

	pmvShader::pmvShader(pmvShader&& other) noexcept 
		:
		m_shader(other.m_shader),
		m_device(other.m_device)
	{
		other.m_shader = VK_NULL_HANDLE;
	}

	pmvShader& pmvShader::operator=(pmvShader&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_shader)
			vkDestroyShaderModule(m_device, m_shader, nullptr);

		m_shader = other.m_shader;
		m_device = other.m_device;

		other.m_shader = VK_NULL_HANDLE;

		return *this;
	}

	pmvShader::~pmvShader() noexcept {
		if (m_shader)
			vkDestroyShaderModule(m_device, m_shader, nullptr);
	}

}