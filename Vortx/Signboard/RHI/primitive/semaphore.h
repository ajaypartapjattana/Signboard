#pragma once

namespace rhi::core {
	class device;
}

#include <vulkan/vulkan.h>

namespace rhi::primitive {
	
	struct semaphore_vkAccess;

	class semaphore {
	public:
		semaphore(const rhi::core::device& device) noexcept;

		semaphore(const semaphore&) = delete;
		semaphore& operator=(const semaphore&) = delete;

		semaphore(semaphore&&) noexcept;
		semaphore& operator=(semaphore&&) noexcept;

		~semaphore() noexcept;

	private:
		friend struct semaphore_vkAccess;

		VkSemaphore m_semaphore = VK_NULL_HANDLE;

		VkDevice m_device = VK_NULL_HANDLE;

	};

}