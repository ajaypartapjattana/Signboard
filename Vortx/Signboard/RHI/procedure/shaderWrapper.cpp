#include "shaderWrapper.h"

#include "Signboard/RHI/primitive/shader.h"
#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/vertexLayout_pAccess.h"

namespace rhi {

	pcdShaderModuleCreate::pcdShaderModuleCreate(const rhi::creDevice& device, VkShaderModuleCreateInfo* pCreateInfo) noexcept
		: 
		r_device(rhi::access::device_pAccess::get(device)),
		_info(fetch_basic(pCreateInfo))
	{

	}

	VkShaderModuleCreateInfo pcdShaderModuleCreate::fetch_basic(VkShaderModuleCreateInfo* pCreateInfo) const noexcept {
		if (pCreateInfo)
			return *pCreateInfo;

		VkShaderModuleCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

		return info;
	}

	pcdShaderModuleCreate& pcdShaderModuleCreate::target_source(const std::vector<uint32_t>& bin) {
		_pBin = &bin;

		_info.pCode = _pBin->data();
		_info.codeSize = _pBin->size() * sizeof(uint32_t);

		return *this;
	}

	VkResult pcdShaderModuleCreate::publish(pmvShader& target) const noexcept {
		if (!_pBin)
			return VK_INCOMPLETE;

		VkResult result = vkCreateShaderModule(r_device, &_info, nullptr, &target.m_shader);
		if (result != VK_SUCCESS)
			return result;

		target._dvc = r_device;

		return VK_SUCCESS;

	}

	void pcdShaderModuleCreate::preset(VkShaderModuleCreateInfo* pCreateInfo) noexcept {
		_info = fetch_basic(pCreateInfo);
	}

	void pcdShaderModuleCreate::reset() noexcept {
		_pBin = nullptr;
		_info = fetch_basic(nullptr);
	}

}