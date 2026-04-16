#include "shaderWrapper.h"

#include "Signboard/RHI/primitive/shader.h"
#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/vertexLayout_pAccess.h"

namespace rhi {

	pcdShaderWrapper::pcdShaderWrapper(const rhi::creDevice& device) noexcept
		: 
		m_Device(rhi::access::device_pAccess::get(device)),
		m_binary(nullptr)
	{

	}

	pcdShaderWrapper& pcdShaderWrapper::setBinary(const std::vector<uint32_t>& bin) {
		m_binary = &bin;

		return *this;
	}

	VkResult pcdShaderWrapper::wrapShaderCode(pmvShader& tgtShader) const {
		if (!m_binary)
			return VK_INCOMPLETE;

		VkShaderModuleCreateInfo shaderInfo{};
		shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderInfo.pCode = m_binary->data();
		shaderInfo.codeSize = m_binary->size() * sizeof(uint32_t);

		VkResult result = vkCreateShaderModule(m_Device, &shaderInfo, nullptr, &tgtShader.m_shader);
		if (result != VK_SUCCESS)
			return result;

		tgtShader._dvc = m_Device;

		return VK_SUCCESS;

	}	

}