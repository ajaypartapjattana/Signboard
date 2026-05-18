#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	struct stdQueues {
		VkQueue graphics;
		VkQueue compute;
		VkQueue transfer;
		VkQueue present;

		stdQueues() noexcept
			:
			graphics(VK_NULL_HANDLE),
			compute(VK_NULL_HANDLE),
			transfer(VK_NULL_HANDLE),
			present(VK_NULL_HANDLE)
		{

		}
	};

	struct stdQueueFamilies {
		uint32_t graphics;
		uint32_t compute;
		uint32_t transfer;
		uint32_t present;
	};

}