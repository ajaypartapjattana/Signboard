#include "shaderWrapper.h"

#include "Signboard/RHI/primitive/shader.h"
#include "Signboard/RHI/core/device_pAccess.h"

namespace rhi {

	pcdShaderWrapper::pcdShaderWrapper(const rhi::creDevice& device) noexcept
		: m_Device(rhi::access::device_pAccess::get(device))
	{

	}

	pcdShaderWrapper& pcdShaderWrapper::set_target(rhi::pmvShader& targetShader) noexcept {
		m_target = &targetShader;

		return *this;
	}

	VkResult pcdShaderWrapper::warpShaderCode(const char* data, size_t size) {
		if (!m_target || !data || !*data)
			return VK_INCOMPLETE;

		if (size % 4 != 0)
			return VK_ERROR_INVALID_SHADER_NV;

		VkShaderModuleCreateInfo shaderInfo{};
		shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderInfo.pCode = reinterpret_cast<const uint32_t*>(data);
		shaderInfo.codeSize = size;

		VkResult result = vkCreateShaderModule(m_Device, &shaderInfo, nullptr, &m_target->m_shader);
		if (result != VK_SUCCESS)
			return result;

		m_target->m_device = m_Device;

		return VK_SUCCESS;

	}

}