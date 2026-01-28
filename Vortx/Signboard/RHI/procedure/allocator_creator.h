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

		rhi::core::allocator create() const ;

	private:
		VkInstance m_instance;
		VkDevice m_device;
		VkPhysicalDevice m_phys;

	};

}