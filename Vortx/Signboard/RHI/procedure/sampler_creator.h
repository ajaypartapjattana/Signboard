#pragma once

namespace rhi::core {
	class device;
}

namespace rhi::primitive {
	class sampler;
}

#include <vulkan/vulkan.h>

namespace rhi::procedure {

	class sampler_creator {
	public:
		sampler_creator(const rhi::core::device& device);

		sampler_creator(const sampler_creator&) = delete;
		sampler_creator& operator=(const sampler_creator&) = delete;

		sampler_creator& set_addressingMode(VkSamplerAddressMode mode);
		sampler_creator& set_filter(VkFilter filter);

		sampler_creator& require_anisotropy();

		VkResult create(rhi::primitive::sampler& target_sampler);

	private:
		VkDevice m_device;
		
		VkBool32 enabled_anisotropy = VK_FALSE;
		float max_anisotropy = 16.0f;

		bool final_anisotropy = false;

		VkSamplerAddressMode final_addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		VkFilter final_filter = VK_FILTER_LINEAR;

	};

}