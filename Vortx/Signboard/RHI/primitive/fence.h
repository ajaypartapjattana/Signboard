#pragma once

namespace rhi::core {
	class device;
}

#include <vulkan/vulkan.h>

namespace rhi::primitive {
	
	struct fence_vkAccess;

	class fence {
	public:
		fence(const rhi::core::device& device) noexcept;
		fence(const rhi::core::device& device, const bool signaled) noexcept;

		fence(const fence&) = delete;
		fence& operator=(const fence&) = delete;

		fence(fence&&) noexcept;
		fence& operator=(fence&&) noexcept;

		~fence() noexcept;

	private:
		void create(const bool signaled);

	private:
		friend struct fence_vkAccess;

		VkFence m_fence = VK_NULL_HANDLE;

		VkDevice m_device = VK_NULL_HANDLE;

	};

}