#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "Signboard/Core/core.h"

namespace rhi {

	class creDevice;

	class pmvCommandPool;
	class pmvCommandBuffer;

	class pcdCommandBufferAllocate {
	public:
		pcdCommandBufferAllocate(const creDevice& device, VkCommandBufferAllocateInfo* pAllocInfo = nullptr) noexcept;

		VkResult target_commandPool(const pmvCommandPool& commandPool) noexcept;
		void secondryBuffers(bool enable = true) noexcept;

		VkResult publish(sgb::span<pmvCommandBuffer> buffers) noexcept;
		void free_buffers(sgb::span<pmvCommandBuffer> buffers) const noexcept;

	private:
		VkCommandBufferAllocateInfo* allot_basic(VkCommandBufferAllocateInfo* pAllocInfo) noexcept;

	private:
		const VkDevice r_device;

		std::unique_ptr<VkCommandBufferAllocateInfo> m_ownedInfo;
		VkCommandBufferAllocateInfo* pInfo;

	};

}