#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi {

	class creAllocator;

	class pmvBuffer;

	class pcdBufferAllocate {
	public:
		pcdBufferAllocate(const rhi::creAllocator& allocator, VkBufferCreateInfo* pCreateInfo = nullptr) noexcept;

		pcdBufferAllocate(const pcdBufferAllocate&) = delete;
		pcdBufferAllocate& operator=(const pcdBufferAllocate&) = delete;

		pcdBufferAllocate& add_usage(VkBufferUsageFlags usage) noexcept;
		pcdBufferAllocate& prefer_memory(VmaMemoryUsage memory) noexcept;
		pcdBufferAllocate& set_allocationFlags(VmaAllocationCreateFlags allocationFlags) noexcept;
		pcdBufferAllocate& size(VkDeviceSize size) noexcept;

		VkResult publish(rhi::pmvBuffer& buffer) const noexcept;

		void preset(VkBufferCreateInfo* pCreateInfo, VmaAllocationCreateInfo* pAllocInfo) noexcept;
		void reset() noexcept;

	private:
		VkBufferCreateInfo fetch_basic(VkBufferCreateInfo* pCreateInfo) const noexcept;

	private:
		VmaAllocator r_allocator;
		const VkPhysicalDeviceMemoryProperties& memProps;

		VkBufferCreateInfo _bufferInfo;
		VmaAllocationCreateInfo _allocInfo;

	};

}