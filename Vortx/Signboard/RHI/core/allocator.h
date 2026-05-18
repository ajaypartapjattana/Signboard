#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi {

	struct _pAccess;

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
		friend struct _pAccess;

		VmaAllocator m_allocator;
		VkPhysicalDeviceMemoryProperties _memProps;

		static const VmaVulkanFunctions m_vkfuncs;

	};

}