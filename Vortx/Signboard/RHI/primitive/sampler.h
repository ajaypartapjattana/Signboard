#pragma once

#include <vulkan/vulkan.h>

namespace rhi::access {
	struct sampler_pAccess;
}

namespace rhi {

	class pcdSamplerCreator;

	class pmvSampler {
	public:
		pmvSampler() noexcept;

		pmvSampler(const pmvSampler&) = delete;
		pmvSampler& operator=(const pmvSampler&) = delete;

		pmvSampler(pmvSampler&&) noexcept;
		pmvSampler& operator=(pmvSampler&&) noexcept;

		~pmvSampler();

	private:
		friend class pcdSamplerCreator;
		friend struct rhi::access::sampler_pAccess;

		VkSampler m_sampler;
		VkDevice _dvc;

	};

}