#pragma once

#include "vulkan/vulkan.h"

namespace rhi::primitive {

	class fence {
	public:
		fence() noexcept;

		fence(const fence&) = delete;
		fence& operator=(const fence&) = delete;

		fence(fence&&) noexcept;
		fence& operator=(fence&&) noexcept;

		~fence() noexcept;

	private:
		VkFence m_fence = VK_NULL_HANDLE;

		VkDevice m_device = VK_NULL_HANDLE;

	};

}