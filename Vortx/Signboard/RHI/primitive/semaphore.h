#pragma once

#include <vulkan/vulkan.h>

namespace rhi::access {
}

namespace rhi {

	struct _pAccess;

	class creDevice;

	class pmvSemaphore {
	public:
		pmvSemaphore() noexcept;
		pmvSemaphore(const rhi::creDevice& device) noexcept;

		pmvSemaphore(const pmvSemaphore&) = delete;
		pmvSemaphore& operator=(const pmvSemaphore&) = delete;

		pmvSemaphore(pmvSemaphore&&) noexcept;
		pmvSemaphore& operator=(pmvSemaphore&&) noexcept;

		~pmvSemaphore() noexcept;

	private:
		friend struct _pAccess;

		VkSemaphore m_semaphore;
		VkDevice r_device;

	};

}