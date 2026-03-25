#pragma once

#include <vulkan/vulkan.h>

namespace rhi {
	
	class creDevice;

	class pmvSampler;

	class pcdSamplerCreator {
	public:
		pcdSamplerCreator(const rhi::creDevice& device) noexcept;

		pcdSamplerCreator(const pcdSamplerCreator&) = delete;
		pcdSamplerCreator& operator=(const pcdSamplerCreator&) = delete;

		pcdSamplerCreator& set_addressingMode(VkSamplerAddressMode mode);
		pcdSamplerCreator& set_filter(VkFilter filter);

		pcdSamplerCreator& require_anisotropy();

		VkResult create(rhi::pmvSampler& target_sampler);

	private:
		VkDevice m_device;
		
		VkSamplerCreateInfo info;

		VkBool32 enabled_anisotropy = VK_FALSE;
		float max_anisotropy = 16.0f;

		bool final_anisotropy = false;

	};

}