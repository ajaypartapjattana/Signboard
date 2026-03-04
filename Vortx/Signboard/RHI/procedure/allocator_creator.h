#pragma once

namespace rhi::core {
	class allocator;

	class instance;
	class device;
};

#include "vulkan/vulkan.h"

namespace rhi::procedure {

	class allocator_creator {
	public:
		allocator_creator(const rhi::core::instance& instance, const rhi::core::device& device);

		VkResult create(rhi::core::allocator& target_allocator) const;

	private:
		VkInstance m_instance = VK_NULL_HANDLE;
		VkDevice m_device = VK_NULL_HANDLE;
		VkPhysicalDevice m_phys = VK_NULL_HANDLE;

	};

}