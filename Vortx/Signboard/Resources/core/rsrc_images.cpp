#include "rsrc_images.h"

rsrc_images::rsrc_images(const rhi::creDevice& device, const rhi::creAllocator& allocator, ctnr::vault_writeAccessor<rhi::pmvImage> imageWrite)
	:
	r_device(device),
	r_allocator(allocator),
	_wrt(imageWrite)
{

}

uint32_t rsrc_images::createImage(const createInfo& info) {
	rhi::pcdImageAllocate pcd{ r_device, r_allocator };
	
	pcd.image_format(info.format);
	pcd.image_extent(info.extent);
	pcd.image_usage(info.usage);

	pcd.allocation_usage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

	auto _ctor = [&](rhi::pmvImage* i) {
		pcd.allocate(*i);
	};

	return _wrt.construct(_ctor);

}

