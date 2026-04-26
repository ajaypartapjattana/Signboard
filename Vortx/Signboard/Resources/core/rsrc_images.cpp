#include "rsrc_images.h"

rsrc_images::rsrc_images(const rhi::creDevice& device, const rhi::creAllocator& allocator, ctnr::vltWrite<rhi::pmvImage> imageWrite)
	:
	m_allocator(device, allocator),
	_wrt(imageWrite)
{

}

uint32_t rsrc_images::createImage(const createInfo& info) {
	m_allocator.image_format(info.format);
	m_allocator.image_extent(info.extent);
	m_allocator.image_usage(info.usage);

	m_allocator.allocation_usage(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

	auto _ctor = [&](rhi::pmvImage* i) {
		m_allocator.allocate(*i);
	};

	return _wrt.construct(_ctor);

}

