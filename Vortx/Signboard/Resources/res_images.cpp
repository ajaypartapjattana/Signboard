#include "res_images.h"

#include <vector>

#include "Signboard/Renderer/RenderBackend/rndr_context_Access.h"

res_images::res_images(const rndr_context& ctx)
	: r_device(rndr_context_Access::get_device(ctx)), r_allocator(rndr_context_Access::get_allocator(ctx))
{

}

void res_images::create_image() {
	rhi::primitive::image l_image;

	rhi::procedure::image_allocator allocator{ r_device, r_allocator };
	allocator.allocate(l_image);

	/*storage::vault_writeAccessor<rhi::primitive::image> write{ m_textureImages };
	write.create(l_image);*/
}