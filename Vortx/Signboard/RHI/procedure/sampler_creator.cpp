#include "sampler_creator.h"

#include "Signboard/RHI/primitive/sampler.h"
#include "Signboard/RHI/core/device_vk.h"

#include <stdexcept>

namespace rhi::procedure {

	sampler_creator::sampler_creator(const rhi::core::device& device)
		: m_device(rhi::core::device_vkAccess::get(device))
	{
		if (device.active_feature(&VkPhysicalDeviceFeatures::samplerAnisotropy)) {
			enabled_anisotropy = VK_TRUE;
		}
	}

	sampler_creator& sampler_creator::set_addressingMode(VkSamplerAddressMode mode) {
		final_addressMode = mode;
		return *this;
	}

	sampler_creator& sampler_creator::set_filter(VkFilter filter) {
		final_filter = filter;
		return *this;
	}

	sampler_creator& sampler_creator::require_anisotropy() {
		final_anisotropy = true;
		return *this;
	}

	VkResult sampler_creator::create(rhi::primitive::sampler& target_sampler) {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		
		samplerInfo.minFilter = final_filter;
		samplerInfo.magFilter = final_filter;

		samplerInfo.addressModeU = final_addressMode;
		samplerInfo.addressModeV = final_addressMode;
		samplerInfo.addressModeW = final_addressMode;

		samplerInfo.anisotropyEnable = final_anisotropy ? enabled_anisotropy : VK_FALSE;
		samplerInfo.maxAnisotropy = final_anisotropy ? max_anisotropy : 1.0f;

		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		samplerInfo.mipLodBias = 0.0f;

		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		samplerInfo.pNext = nullptr;

		VkSampler vk_sampler = VK_NULL_HANDLE;
		VkResult result = vkCreateSampler(m_device, &samplerInfo, nullptr, &vk_sampler);
		if (result != VK_SUCCESS)
			return result;

		target_sampler.m_device = m_device;
		target_sampler.m_sampler = vk_sampler;

		return result;
	}

}
