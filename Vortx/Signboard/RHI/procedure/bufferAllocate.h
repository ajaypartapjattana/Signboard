#pragma once

#include <memory>

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi {

	class allocator;

	class pmvBuffer;

	class pcdBufferAllocate {
	public:
		pcdBufferAllocate(const allocator& allocator, VkBufferCreateInfo* pCreateInfo = nullptr, VmaAllocationCreateInfo* pAllocInfo = nullptr) noexcept;

		pcdBufferAllocate(const pcdBufferAllocate&) = delete;
		pcdBufferAllocate& operator=(const pcdBufferAllocate&) = delete;

		void usage(VkBufferUsageFlags usage) noexcept;
		void prefer_memory(VmaMemoryUsage memory) noexcept;
		void set_allocationFlags(VmaAllocationCreateFlags allocationFlags) noexcept;
		void size(VkDeviceSize size) noexcept;

		VkResult publish(pmvBuffer& buffer) const noexcept;

		void preset(VkBufferCreateInfo* pCreateInfo, VmaAllocationCreateInfo* pAllocInfo) noexcept;
		void reset() noexcept;

	private:
		VkBufferCreateInfo* allot_basic(VkBufferCreateInfo* pCreateInfo) noexcept;
		VmaAllocationCreateInfo* allot_allocInfo(VmaAllocationCreateInfo* pAllocInfo) noexcept;

	private:
		const VmaAllocator r_allocator;
		const VkPhysicalDeviceMemoryProperties& memProps;

		std::unique_ptr<VkBufferCreateInfo> m_ownedBufferInfo;
		VkBufferCreateInfo* _bufferInfo;

		std::unique_ptr<VmaAllocationCreateInfo> m_ownedAllocInfo;
		VmaAllocationCreateInfo* _allocInfo;

	};

}