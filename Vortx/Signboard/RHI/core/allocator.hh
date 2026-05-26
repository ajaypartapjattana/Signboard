#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi {

	class allocator {
	private:
		VmaAllocator m_allocator = VK_NULL_HANDLE;

		static const VmaVulkanFunctions m_vkfuncs;

	public:
		allocator() = default;
		allocator(VmaAllocatorCreateInfo* pCreateInfo) {
			create(pCreateInfo);

		}
		allocator(const allocator&) = delete;
		allocator(allocator&&) noexcept;

		allocator& operator=(const allocator&) = delete;
		allocator& operator=(allocator&&) noexcept;

		~allocator() noexcept;

		VkResult create(VmaAllocatorCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	};

}