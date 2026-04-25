#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi::access{
	struct image_pAccess;
}

namespace rhi {

	class pcdImageAllocate;
	class pcdSwapchainImageAllocate;

	class pmvImage {
	public:
		pmvImage() noexcept;

		pmvImage(const pmvImage&) = delete;
		pmvImage& operator=(const pmvImage&) = delete;

		pmvImage(pmvImage&&) noexcept;
		pmvImage& operator=(pmvImage&&) noexcept;

		~pmvImage() noexcept;

		void reset() noexcept;

	private:
		friend class pcdImageAllocate;
		friend class pcdSwapchainImageAllocate;
		friend struct rhi::access::image_pAccess;

		VkImage m_image;
		VmaAllocation m_allocation;
		VkImageView m_view;

		VkExtent3D extent;
		VkFormat format;

		VkDevice r_device;
		VmaAllocator r_allocator;

	};

}