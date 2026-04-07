#pragma once

#include <vulkan/vulkan.h>

namespace rhi::access {
	struct fence_pAccess;
}

namespace rhi {

	class creDevice;

	class pmvFence {
	public:
		pmvFence(const rhi::creDevice& device) noexcept;
		pmvFence(const rhi::creDevice& device, const bool signaled) noexcept;

		pmvFence(const pmvFence&) = delete;
		pmvFence& operator=(const pmvFence&) = delete;

		pmvFence(pmvFence&&) noexcept;
		pmvFence& operator=(pmvFence&&) noexcept;

		~pmvFence() noexcept;

	private:
		void create(const bool signaled);

	private:
		friend struct rhi::access::fence_pAccess;

		VkFence m_fence;
		VkDevice _dvc;

	};

}