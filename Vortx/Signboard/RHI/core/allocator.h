#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi::access {
	struct allocator_pAccess;
}

namespace rhi {

	class creInstance;
	class creDevice;

	class creAllocator {
	public:
		creAllocator(const creInstance&, const creDevice&);

		creAllocator(const creAllocator&) = delete;
		creAllocator& operator=(const creAllocator&) = delete;

		creAllocator(creAllocator&&) noexcept;
		creAllocator& operator=(creAllocator&&) noexcept;

		~creAllocator() noexcept;

	private:
		friend struct rhi::access::allocator_pAccess;

		VmaAllocator m_allocator;
		static const VmaVulkanFunctions m_vkfuncs;

	};

}