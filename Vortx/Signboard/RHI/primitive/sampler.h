#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	struct _pAccess;

	class pcdSamplerCreate;

	class pmvSampler {
	public:
		pmvSampler() noexcept;

		pmvSampler(const pmvSampler&) noexcept;
		pmvSampler& operator=(const pmvSampler&) noexcept;

		pmvSampler(pmvSampler&&) noexcept;
		pmvSampler& operator=(pmvSampler&&) noexcept;

		~pmvSampler() noexcept;

		void reset() noexcept;

	private:
		friend class pcdSamplerCreate;
		friend struct _pAccess;

		VkSampler m_sampler;

		VkDevice r_device;

	};

}