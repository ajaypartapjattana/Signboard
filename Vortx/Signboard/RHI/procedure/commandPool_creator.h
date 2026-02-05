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
		struct pool_requirement {
			uint32_t family;
			VkQueueFlags capabilities;
			bool present_supported;
		};

		commandPool_creator(const rhi::core::device& device);

		uint32_t get_requiredPoolCount() noexcept;
		const std::vector<pool_requirement>& get_poolRequirements() noexcept;

		struct pool_reqirement {
			uint32_t family;
			VkQueueFlags capabilities;
			bool present_supported;
		};

		const std::vector<pool_reqirement>& get_poolRequirements() noexcept;

		VkResult create(rhi::core::commandPool* pools, uint32_t poolCount);

	private:
		void collect_poolRequirements() noexcept;

	private:
		const rhi::core::device& m_device;

		std::vector<pool_requirement> m_requirements;

	};

}
