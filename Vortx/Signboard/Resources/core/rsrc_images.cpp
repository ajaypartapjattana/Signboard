#include "rsrc_images.h"

rsrc_images::rsrc_images(const rhi::creDevice& device, const rhi::creAllocator& allocator)
	:
	r_device(device),
	r_allocator(allocator),

	m_writer(m_textureImages)
{

}

uint32_t rsrc_images::createImage(const createInfo& info) {
	rhi::pcdImageAllocator prcdr{ r_device, r_allocator };
	
	prcdr.set_format(info.format);
	prcdr.set_extent(info.extent);
	prcdr.set_usage(info.usage);
	prcdr.set_aspect(info.aspect);

	auto builder = [&](rhi::pmvImage* i) {
		prcdr.allocate(*i);
	};

	return m_writer.construct(builder);

}

ctnr::vltView_const<rhi::pmvImage> rsrc_images::read_Images() const noexcept {
	return ctnr::vltView<rhi::pmvImage>{ m_textureImages }.with_static();
}
