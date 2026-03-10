#pragma once

#include "image.h"

namespace rhi::primitive {

	struct image_vkAccess {
		
		static VkImage get(const image& i) noexcept {
			return i.m_image;
		}

		static VkImageView get_view(const image& i) noexcept {
			return i.m_view;
		}

		static VmaAllocation get_allocation(const image& i) noexcept {
			return i.m_allocation;
		}

		static VkExtent3D get_extent(const image& i) noexcept {
			return i.m_extent;
		}

		static VkFormat get_format(const image& i) noexcept {
			return i.m_format;
		}

	};

}