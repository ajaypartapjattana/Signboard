#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi::core {

	class instance;
	class device;

	struct allocator_vmaAccess;

	class allocator {
	public:
		allocator(const instance&, const device&);

		allocator(const allocator&) = delete;
		allocator& operator=(const allocator&) = delete;

		allocator(allocator&&) noexcept;
		allocator& operator=(allocator&&) noexcept;

		~allocator() noexcept;

		VmaAllocator native_allocator() const noexcept;

	private:
		friend struct allocator_vmaAccess;

		VmaAllocator m_allocator = VK_NULL_HANDLE;
		VmaVulkanFunctions m_vkfuncs{};

	};

}