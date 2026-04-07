#pragma once

#include <vulkan/vulkan.h>

namespace rhi::access {
	struct commandPool_pAccess;
}

namespace rhi {

	class pcdCommandPoolCreator;

	class pmvCommandPool {
	public:
		pmvCommandPool() noexcept;

		pmvCommandPool(const pmvCommandPool&) = delete;
		pmvCommandPool& operator=(const pmvCommandPool&) = delete;

		pmvCommandPool(pmvCommandPool&&) noexcept;
		pmvCommandPool& operator=(pmvCommandPool&&) noexcept;

		~pmvCommandPool() noexcept;

		VkCommandPool native_commandPool() const noexcept;

	private:
		friend class pcdCommandPoolCreator;
		friend struct rhi::access::commandPool_pAccess;

		VkCommandPool m_commandPool = VK_NULL_HANDLE;
		uint32_t m_queueFamilyIndex = 0;

		VkDevice _dvc = VK_NULL_HANDLE;
	};

}