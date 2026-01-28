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

		sampler_creator& set_addressingMode_REPEAT();
		sampler_creator& set_addressingMode_CLAMP();
		sampler_creator& set_addressingMode_MIRROR();

		sampler_creator& set_filter_NEAREST();
		sampler_creator& set_filter_LINEAR();

		sampler_creator& require_anisotropy();

		rhi::primitive::sampler create();

	private:
		void set_addressingMode(VkSamplerAddressMode mode);
		void set_filter(VkFilter filter);

		void reset_creator();

	private:
		VkDevice m_device;
		
		VkBool32 enabled_anisotropy = VK_FALSE;
		float max_anisotropy = 1.0f;

		bool final_anisotropy = false;

		VkSamplerAddressMode final_addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		VkFilter final_filter = VK_FILTER_LINEAR;

	};

}