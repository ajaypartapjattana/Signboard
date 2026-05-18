#pragma once

#include <vulkan/vulkan.h>

namespace rhi {
	
	struct _pAccess;

	class creDevice;

	class pmvFence {
	public:
		pmvFence(const creDevice& device) noexcept;
		pmvFence(const creDevice& device, const bool signaled) noexcept;

		pmvFence(const pmvFence&) = delete;
		pmvFence& operator=(const pmvFence&) = delete;

		pmvFence(pmvFence&&) noexcept;
		pmvFence& operator=(pmvFence&&) noexcept;

		~pmvFence() noexcept;

	private:
		void create(const bool signaled);

	private:
		friend struct _pAccess;

		VkFence m_fence;
		VkDevice r_device;

	};

}