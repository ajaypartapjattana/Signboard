#pragma once

namespace rhi::core {
	class device;
	class allocator;
}

namespace rhi::primitive {
	class image;
}

#include <vma.h>

namespace rhi::procedure {

	class image_allocator {
	public:
		image_allocator(const rhi::core::device& deivce, const rhi::core::allocator& allocator);

		image_allocator(const image_allocator&) = delete;
		image_allocator& operator=(const image_allocator&) = delete;

		image_allocator& set_usage_colorAttachment();
		image_allocator& set_usage_depthStencil();
		image_allocator& set_usage_transferDst();
		image_allocator& set_usage_sampled();

		image_allocator& set_format_R8G8B8A8();

		image_allocator& set_extent(uint32_t w, uint32_t h);

		[[nodiscard]] rhi::primitive::image allocate();

	private:
		image_allocator& set_usage(VkImageUsageFlags usage);
		image_allocator& set_format(VkFormat format);

		void reset_allocatorState();

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VmaAllocator m_allocator = VK_NULL_HANDLE;

		VkImageUsageFlags final_usage{};
		
		VkFormat final_format{};

		VkExtent3D final_extent{};

		VkImageAspectFlags final_aspect{};

	};

}