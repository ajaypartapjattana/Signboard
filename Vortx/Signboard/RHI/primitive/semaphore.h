#pragma once

#include <vulkan/vulkan.h>

namespace rhi::access {
	struct semaphore_pAccess;
}

namespace rhi {

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
		friend struct rhi::access::semaphore_pAccess;

		VkSemaphore m_semaphore;
		VkDevice _dvc;

	};

}