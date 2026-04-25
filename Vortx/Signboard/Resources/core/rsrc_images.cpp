#include "rsrc_images.h"

rsrc_images::rsrc_images(const rhi::creDevice& device, const rhi::creAllocator& allocator, ctnr::vault_writeAccessor<rhi::pmvImage> imageWrite)
	:
	r_device(device),
	r_allocator(allocator),
	m_writer(imageWrite)
{

}

uint32_t rsrc_images::createImage(const createInfo& info) {
	rhi::pcdImageAllocate prcdr{ r_device, r_allocator };
	
	prcdr.set_format(info.format);
	prcdr.set_extent(info.extent);
	prcdr.set_usage(info.usage);
	prcdr.set_aspect(info.aspect);

	auto builder = [&](rhi::pmvImage* i) {
		prcdr.allocate(*i);
	};

	return m_writer.construct(builder);

}

