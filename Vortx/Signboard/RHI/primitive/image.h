#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi::access{
	struct image_pAccess;
}

namespace rhi {

	class pcdImageAllocator;

	class pmvImage {
	public:
		pmvImage() noexcept;

		pmvImage(const pmvImage&) = delete;
		pmvImage& operator=(const pmvImage&) = delete;

		pmvImage(pmvImage&&) noexcept;
		pmvImage& operator=(pmvImage&&) noexcept;

		~pmvImage() noexcept;

	private:
		friend class pcdImageAllocator;
		friend struct rhi::access::image_pAccess;

		VkImage m_image;
		VmaAllocation _alloc;
		VkImageView m_view;

		VkExtent3D m_extent;
		VkFormat m_format;

		VkDevice _dvc;
		VmaAllocator _allctr;

	};

}