#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdShaderModuleCreate::pcdShaderModuleCreate(const creDevice& device, VkShaderModuleCreateInfo* pCreateInfo) noexcept
		: 
		r_device(_pAccess::extract(device)),
		pInfo(fetch_basic(pCreateInfo))
	{

	}

	VkShaderModuleCreateInfo* pcdShaderModuleCreate::fetch_basic(VkShaderModuleCreateInfo* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<VkShaderModuleCreateInfo>();

		VkShaderModuleCreateInfo* _info = m_ownedInfo.get();
		_info->sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

		return _info;
	}

	void pcdShaderModuleCreate::target_source(sgb::span<const uint32_t> bin) noexcept {
		pInfo->pCode = bin.data();
		pInfo->codeSize = bin.size() * sizeof(uint32_t);
	}

	VkResult pcdShaderModuleCreate::publish(pmvShader& target, VkShaderStageFlagBits stage) const noexcept {
		if (!pInfo->pCode)
			return VK_INCOMPLETE;

		VkShaderModule _shader;
		VkResult result = vkCreateShaderModule(r_device, pInfo, nullptr, &_shader);
		if (result != VK_SUCCESS)
			return result;

		target.reset();

		target.m_shader = _shader;
		target.stage = stage;
		target.r_device = r_device;

		return VK_SUCCESS;

	}

	void pcdShaderModuleCreate::preset(VkShaderModuleCreateInfo* pCreateInfo) noexcept {
		m_ownedInfo.reset();
		pInfo = fetch_basic(pCreateInfo);
	}

	void pcdShaderModuleCreate::reset() noexcept {
		m_ownedInfo.reset();
		pInfo = fetch_basic(nullptr);
	}

}