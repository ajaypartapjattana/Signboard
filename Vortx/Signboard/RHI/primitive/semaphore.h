#pragma once

#include "vulkan/vulkan.h"

namespace rhi::primitive {

	class semaphore {
	public:
		semaphore() noexcept;

		semaphore(const semaphore&) = delete;
		semaphore& operator=(const semaphore&) = delete;

		semaphore(semaphore&&) noexcept;
		semaphore& operator=(semaphore&&) noexcept;

		~semaphore() noexcept;

	private:
		VkSemaphore m_semaphore = VK_NULL_HANDLE;

		VkDevice m_device = VK_NULL_HANDLE;

	};

}