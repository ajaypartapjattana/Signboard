#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi{

	pcdSamplerCreate::pcdSamplerCreate(const device& device, VkSamplerCreateInfo* pCreateInfo) noexcept
		: 
		r_device(_pAccess::extract(device)),
		pInfo(allot_basic(pCreateInfo))
	{
		if (device.active_feature(&VkPhysicalDeviceFeatures::samplerAnisotropy)) {
			enabled_anisotropy = VK_TRUE;
		}
	}

	VkSamplerCreateInfo* pcdSamplerCreate::allot_basic(VkSamplerCreateInfo* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<VkSamplerCreateInfo>();

		VkSamplerCreateInfo* _info = m_ownedInfo.get();
		_info->sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		_info->maxAnisotropy = 1.0f;

		return _info;
	}

	void pcdSamplerCreate::addressing(VkSamplerAddressMode mode) noexcept {
		pInfo->addressModeU = mode;
		pInfo->addressModeV = mode;
		pInfo->addressModeW = mode;
	}

	void pcdSamplerCreate::filter(VkFilter filter, VkSamplerMipmapMode mode, VkBorderColor color) noexcept {
		pInfo->minFilter = filter;
		pInfo->magFilter = filter;

		pInfo->mipmapMode = mode;

		pInfo->borderColor = color;
	}

	void pcdSamplerCreate::Lod(float min, float max, float bias) noexcept {
		pInfo->minLod = min;
		pInfo->maxLod = max;
		pInfo->mipLodBias = bias;
	}

	void pcdSamplerCreate::enableAnisotropy(float max) noexcept {
		if (!enabled_anisotropy)
			return;

		pInfo->anisotropyEnable = VK_TRUE;
		pInfo->maxAnisotropy = max;
	}

	void pcdSamplerCreate::enableCompare(VkCompareOp Op) noexcept {
		if (Op == VK_COMPARE_OP_NEVER) {
			pInfo->compareEnable = VK_FALSE;
			return;
		}

		pInfo->compareEnable = VK_TRUE;
		pInfo->compareOp = Op;
	}

	void pcdSamplerCreate::UnnormalizedCoordinates(bool enable) noexcept {
		pInfo->unnormalizedCoordinates = enable ? VK_TRUE : VK_FALSE;
	}

	VkResult pcdSamplerCreate::publish(pmvSampler& target) const noexcept {
		VkSampler _sampler;
		VkResult result = vkCreateSampler(r_device, pInfo, nullptr, &_sampler);
		if (result != VK_SUCCESS)
			return result;

		target.reset();

		target.m_sampler = _sampler;
		target.r_device = r_device;

		return result;
	}

	void pcdSamplerCreate::preset(VkSamplerCreateInfo* pCreateInfo) noexcept {
		m_ownedInfo.reset();
		pInfo = allot_basic(pCreateInfo);
	}

	void pcdSamplerCreate::reset() noexcept {
		m_ownedInfo.reset();
		pInfo = allot_basic(nullptr);
	}

}
