#pragma once

#include "Signboard/Core/Containers/storage.h"
#include "Signboard/RHI/rhi.h"

class rsrc_images {
public:
	rsrc_images(const rhi::creDevice& device, const rhi::creAllocator& allocator, ctnr::vltWrite<rhi::pmvImage> imageWrite);

	struct createInfo {
		VkFormat format;
		VkExtent3D extent;
		VkImageUsageFlags usage;
		VkImageAspectFlags aspect;
	};

	uint32_t createImage(const createInfo& info);

private:
	rhi::pcdImageAllocate m_allocator;
	ctnr::vltWrite<rhi::pmvImage> _wrt;

};