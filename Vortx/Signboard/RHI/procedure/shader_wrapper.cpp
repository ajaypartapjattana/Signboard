#include "shader_wrapper.h"

#include "Signboard/RHI/primitive/shader.h"
#include "Signboard/RHI/core/device_vk.h"

namespace rhi::procedure {

	shader_wrapper::shader_wrapper(const rhi::core::device& device) noexcept
		: m_Device(rhi::core::device_vkAccess::get(device))
	{

	}

	shader_wrapper& shader_wrapper::set_target(rhi::primitive::shader& targetShader) noexcept {
		m_target = &targetShader;

		return *this;
	}

	VkResult shader_wrapper::wrap_shaderCode(const char* data, size_t size) {
		if (!m_target || !data || !*data)
			return VK_INCOMPLETE;

		if (size % 4 != 0)
			return VK_ERROR_INVALID_SHADER_NV;

		VkShaderModuleCreateInfo shaderInfo{};
		shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderInfo.pCode = reinterpret_cast<const uint32_t*>(*data);
		shaderInfo.codeSize = size;

		VkShaderModule vk_shaderModule = VK_NULL_HANDLE;
		VkResult result = vkCreateShaderModule(m_Device, &shaderInfo, nullptr, &vk_shaderModule);

		m_target->m_shader = vk_shaderModule;
		m_target->m_device = m_Device;

		return result;

	}

}