#include "shader.h"

namespace rhi {

	pmvShader::pmvShader() noexcept
		: 
		m_shader(VK_NULL_HANDLE),
		stage(VK_SHADER_STAGE_VERTEX_BIT),
		r_device(VK_NULL_HANDLE)
	{
	
	}

	pmvShader::pmvShader(const pmvShader& other) noexcept 
		:
		m_shader(other.m_shader),
		stage(other.stage),
		r_device(VK_NULL_HANDLE)
	{

	}

	pmvShader& pmvShader::operator=(const pmvShader& other) noexcept {
		if (this == &other)
			return *this;

		if (r_device)
			vkDestroyShaderModule(r_device, m_shader, nullptr);

		m_shader = other.m_shader;
		stage = other.stage;
		r_device = VK_NULL_HANDLE;

		return *this;
	}

	pmvShader::pmvShader(pmvShader&& other) noexcept 
		:
		m_shader(other.m_shader),
		stage(other.stage),
		r_device(other.r_device)
	{
		other.r_device = VK_NULL_HANDLE;
	}

	pmvShader& pmvShader::operator=(pmvShader&& other) noexcept {
		if (this == &other)
			return *this;

		if (r_device)
			vkDestroyShaderModule(r_device, m_shader, nullptr);

		m_shader = other.m_shader;
		stage = other.stage;
		r_device = other.r_device;

		other.r_device = VK_NULL_HANDLE;

		return *this;
	}

	pmvShader::~pmvShader() noexcept {
		if (r_device)
			vkDestroyShaderModule(r_device, m_shader, nullptr);
	}

	void pmvShader::reset() noexcept {
		if (r_device) {
			vkDestroyShaderModule(r_device, m_shader, nullptr);
			m_shader = VK_NULL_HANDLE;
		}

		r_device = VK_NULL_HANDLE;
	}

}