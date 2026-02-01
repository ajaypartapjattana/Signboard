#pragma once

#include "image.h"

namespace rhi::primitive {

	struct image_vkAccess {
		
		static VkImage get(const image& i) {
			return i.m_image;
		}

		static VkImageView get_view(const image& i) {
			return i.m_view;
		}

		static VmaAllocation get_allocation(const image& i) {
			return i.m_allocation;
		}

		static VkFormat get_format(const image& i) {
			return i.m_format;
		}

	};

}