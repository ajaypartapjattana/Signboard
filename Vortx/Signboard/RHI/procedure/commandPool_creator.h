#pragma once

namespace rhi::core {
	class commandPool;

	class device;
}

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::procedure {

	class commandPool_creator {
	public:
		commandPool_creator(const rhi::core::device& device);

		uint32_t get_requiredPoolCount() noexcept;

		VkResult create(rhi::core::commandPool* pools, uint32_t poolCount);

	private:
		const rhi::core::device& m_device;

		std::vector<uint32_t> m_families;

	};

}