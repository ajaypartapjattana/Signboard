#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	template <typename Derived>
	struct vulkan_trait_base {
		
		using _Ty = typename Derived::handle_type;
		using _resultTy = typename Derived::result_type;

		static constexpr _Ty null() noexcept {
			return VK_NULL_HANDLE;
		}

		static constexpr _resultTy success() noexcept {
			return VK_SUCCESS;
		}

	};

}