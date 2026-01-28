#include "sampler_creator.h"

#include "Signboard/RHI/primitive/sampler.h"
#include "Signboard/RHI/core/device_vk.h"

#include <stdexcept>

namespace rhi::procedure {

	sampler_creator::sampler_creator(const rhi::core::device& device)
		: m_device(rhi::core::device_vkAccess::get(device))
	{
		reset_creator();

		if (rhi::core::device_vkAccess::get_featureEnabled(device, &VkPhysicalDeviceFeatures::samplerAnisotropy)) {
			enabled_anisotropy = VK_TRUE;
			max_anisotropy = rhi::core::device_vkAccess::get_properties(device).limits.maxSamplerAnisotropy;
		}

	}

	sampler_creator& sampler_creator::set_addressingMode_REPEAT() {
		set_addressingMode(VK_SAMPLER_ADDRESS_MODE_REPEAT);
		return *this;
	}

	sampler_creator& sampler_creator::set_addressingMode_CLAMP() {
		set_addressingMode(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
		return *this;
	}

	sampler_creator& sampler_creator::set_addressingMode_MIRROR() {
		set_addressingMode(VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
		return *this;
	}

	sampler_creator& sampler_creator::set_filter_LINEAR() {
		set_filter(VK_FILTER_LINEAR);
		return *this;
	}

	sampler_creator& sampler_creator::set_filter_NEAREST() {
		set_filter(VK_FILTER_NEAREST);
		return *this;
	}

	sampler_creator& sampler_creator::require_anisotropy() {
		final_anisotropy = true;
		return *this;
	}

	rhi::primitive::sampler sampler_creator::create() {
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
		if (vkCreateSampler(m_device, &samplerInfo, nullptr, &vk_sampler) != VK_SUCCESS)
			throw std::runtime_error("FAILED: sampler_creation!");

		rhi::primitive::sampler l_sampler;
		l_sampler.m_device = m_device;
		l_sampler.m_sampler = vk_sampler;

		return l_sampler;
	}

	void sampler_creator::set_addressingMode(VkSamplerAddressMode mode) {
		final_addressMode = mode;
	}

	void sampler_creator::set_filter(VkFilter filter) {
		final_filter = filter;
	}

	void sampler_creator::reset_creator() {
		final_addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		final_filter = VK_FILTER_LINEAR;
		final_anisotropy = false;
		max_anisotropy = 1.0f;
	}

}
