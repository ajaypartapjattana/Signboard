#pragma once

#include "image.h"

namespace rhi::access {

	struct image_pAccess {
		
		static VkImage extract(const rhi::pmvImage& i) noexcept {
			return i.m_image;
		}

		static VkImageView get_view(const rhi::pmvImage& i) noexcept {
			return i.m_view;
		}

		static VmaAllocation get_allocation(const rhi::pmvImage& i) noexcept {
			return i.m_allocation;
		}

		static VkExtent3D get_extent(const rhi::pmvImage& i) noexcept {
			return i.extent;
		}

		static VkFormat get_format(const rhi::pmvImage& i) noexcept {
			return i.format;
		}

	};

}