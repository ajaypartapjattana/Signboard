#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi {

	struct _pAccess;

	class pcdImageAllocate;
	class pcdImageViewCreate;
	class pcdSwapchainImageAllocate;

	class pmvImage {
	public:
		pmvImage() noexcept;

		pmvImage(const pmvImage&) noexcept;
		pmvImage& operator=(const pmvImage&) noexcept;

		pmvImage(pmvImage&&) noexcept;
		pmvImage& operator=(pmvImage&&) noexcept;

		~pmvImage() noexcept;

		VkFormat native_format() const noexcept;
		VkExtent3D native_extent() const noexcept;

		void reset_view() noexcept;
		void reset() noexcept;

	private:
		friend class pcdImageAllocate;
		friend class pcdImageViewCreate;
		friend class pcdSwapchainImageAllocate;
		friend struct _pAccess;

		VkImage m_image;
		VmaAllocation m_allocation;
		VkImageView m_view;

		VkExtent3D extent;
		VkFormat format;
		
		uint32_t mip_levels;
		uint32_t array_layers;

		VkDevice r_device;
		VmaAllocator r_allocator;

	};

}