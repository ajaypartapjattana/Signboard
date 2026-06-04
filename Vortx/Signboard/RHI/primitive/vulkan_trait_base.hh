#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	struct vulkan_primitive_base {
		using parent_type = VkDevice;
		using result_type = VkResult;

		static constexpr auto null() noexcept {
			return VK_NULL_HANDLE;
		}

		static constexpr parent_type null_root() noexcept {
			return VK_NULL_HANDLE;
		}

		static constexpr result_type success() noexcept {
			return VK_SUCCESS;
		}

	};

}