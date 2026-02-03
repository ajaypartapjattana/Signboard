#pragma once

namespace rhi::procedure {
	class commandPool_creator;
}

#include <vulkan/vulkan.h>

namespace rhi::core {

	struct commandPool_vkAccess;

	class commandPool {
	public:
		commandPool() noexcept;

		commandPool(const commandPool&) = delete;
		commandPool& operator=(const commandPool&) = delete;

		commandPool(commandPool&&) noexcept;
		commandPool& operator=(commandPool&&) noexcept;

		~commandPool() noexcept;

		VkCommandPool native_commandPool() const noexcept;

	private:
		friend class rhi::procedure::commandPool_creator;
		friend struct commandPool_vkAccess;

		VkCommandPool m_commandPool = VK_NULL_HANDLE;
		uint32_t m_queueFamilyIndex;

		VkDevice m_device = VK_NULL_HANDLE;
	};

}