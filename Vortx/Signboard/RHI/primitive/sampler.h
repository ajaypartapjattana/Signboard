#pragma once

namespace rhi::procedure {
	class sampler_creator;
}

#include <vulkan/vulkan.h>

namespace rhi::primitive {

	struct sampler_vkAccess;

	class sampler {
	public:
		sampler(const sampler&) = delete;
		sampler& operator=(const sampler&) = delete;

		sampler(sampler&&) noexcept;
		sampler& operator=(sampler&&) noexcept;

		~sampler();

		VkSampler native_sampler() const noexcept;

	private:
		friend class rhi::procedure::sampler_creator;
		friend struct sampler_vkAccess;

		sampler() = default;

		VkSampler m_sampler;

		VkDevice m_device;


	};

}