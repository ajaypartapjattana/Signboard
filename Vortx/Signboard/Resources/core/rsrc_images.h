#pragma once

#include "Signboard/Core/Containers/storage.h"
#include "Signboard/RHI/rhi.h"

class rsrc_images {
public:
	rsrc_images(const rhi::creDevice& device, const rhi::creAllocator& allocator);

	struct createInfo {
		VkFormat format;
		VkExtent3D extent;
		VkImageUsageFlags usage;
		VkImageAspectFlags aspect;
	};

	uint32_t createImage(const createInfo& info);

	ctnr::vltView_const<rhi::pmvImage> read_Images() const noexcept;

private:
	const rhi::creDevice& r_device;
	const rhi::creAllocator& r_allocator;

	ctnr::vault<rhi::pmvImage> m_textureImages;
	ctnr::vault_writeAccessor<rhi::pmvImage> m_writer;

};