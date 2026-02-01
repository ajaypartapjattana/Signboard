#pragma once

namespace rhi::procedure {
	class allocator_creator;
}

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi::core {

	struct allocator_vmaAccess;

	class allocator {
	public:
		allocator(const allocator&) = delete;
		allocator& operator=(const allocator&) = delete;

		allocator(allocator&&) noexcept;
		allocator& operator=(allocator&&) noexcept;

		~allocator() noexcept;

		VmaAllocator native_allocator() const noexcept;

	private:
		friend class rhi::procedure::allocator_creator;
		friend struct allocator_vmaAccess;

		allocator() = default;

		VmaAllocator m_allocator = VK_NULL_HANDLE;
		VmaVulkanFunctions m_vkfuncs{};

	};

}