#include "samplerCreator.h"

#include "Signboard/RHI/primitive/sampler.h"
#include "Signboard/RHI/core/device_pAccess.h"

#include <stdexcept>

namespace rhi{

	pcdSamplerCreator::pcdSamplerCreator(const rhi::creDevice& device) noexcept
		: 
		_dvc(rhi::access::device_pAccess::extract(device)),
		info()
	{
		info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

		if (device.active_feature(&VkPhysicalDeviceFeatures::samplerAnisotropy)) {
			enabled_anisotropy = VK_TRUE;
		}
	}

	pcdSamplerCreator& pcdSamplerCreator::set_addressingMode(VkSamplerAddressMode mode) {
		info.addressModeU = mode;
		info.addressModeV = mode;
		info.addressModeW = mode;

		return *this;
	}

	pcdSamplerCreator& pcdSamplerCreator::set_filter(VkFilter filter) {
		info.minFilter = filter;
		info.magFilter = filter;

		return *this;
	}

	pcdSamplerCreator& pcdSamplerCreator::require_anisotropy() {
		info.anisotropyEnable = final_anisotropy ? enabled_anisotropy : VK_FALSE;
		info.maxAnisotropy = final_anisotropy ? max_anisotropy : 1.0f;

		return *this;
	}

	VkResult pcdSamplerCreator::create(rhi::pmvSampler& target_sampler) {
		info.compareEnable = VK_FALSE;
		info.compareOp = VK_COMPARE_OP_ALWAYS;

		info.minLod = 0.0f;
		info.maxLod = 0.0f;
		info.mipLodBias = 0.0f;

		info.unnormalizedCoordinates = VK_FALSE;

		info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		info.pNext = nullptr;

		VkSampler vk_sampler = VK_NULL_HANDLE;
		VkResult result = vkCreateSampler(_dvc, &info, nullptr, &vk_sampler);
		if (result != VK_SUCCESS)
			return result;

		target_sampler._dvc = _dvc;
		target_sampler.m_sampler = vk_sampler;

		return result;
	}

}
