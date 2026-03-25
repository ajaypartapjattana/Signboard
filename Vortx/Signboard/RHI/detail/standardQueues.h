#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	struct standardQueues {
		VkQueue graphics;
		VkQueue compute;
		VkQueue transfer;
		VkQueue present;

		standardQueues() noexcept
			:
			graphics(VK_NULL_HANDLE),
			compute(VK_NULL_HANDLE),
			transfer(VK_NULL_HANDLE),
			present(VK_NULL_HANDLE)
		{

		}
	};

	struct standardQueueFamilies {
		uint32_t graphics;
		uint32_t compute;
		uint32_t transfer;
		uint32_t present;

		standardQueueFamilies() noexcept
			:
			graphics(UINT32_MAX),
			compute(UINT32_MAX),
			transfer(UINT32_MAX),
			present(UINT32_MAX)
		{

		}
	};

}