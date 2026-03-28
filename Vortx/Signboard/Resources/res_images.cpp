#include "res_images.h"

#include <vector>

#include "Signboard/Renderer/RenderBackend/rndr_context_Access.h"

rsrc_images::rsrc_images(const rndr_context& ctx)
	:
	r_device(rndr_context_Access::get_device(ctx)),
	r_allocator(rndr_context_Access::get_allocator(ctx)),

	m_writer(m_textureImages)
{

}

storage::storage_handle rsrc_images::createImage(const createInfo& info) {
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

storage::vault_readAccessor<rhi::pmvImage> rsrc_images::get_imageReadAccess() const noexcept {
	return storage::vault_readAccessor<rhi::pmvImage>{ m_textureImages };
}
