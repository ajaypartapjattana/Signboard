#include "res_images.h"

#include <vector>

res_images::res_images(const rhi::core::device& device, const rhi::core::allocator& allocator)
	: m_device(device), m_allocator(allocator)
{

}

void res_images::create_image() {
	rhi::primitive::image l_image;

	rhi::procedure::image_allocator allocator{ m_device, m_allocator };
	allocator.allocate(l_image);

	storage::vault_writeAccessor<rhi::primitive::image> write{ m_textureImages };
	write.create(l_image);
}

void res_images::wrap_swapchainImages(const rhi::core::swapchain& swapchain) {
	rhi::procedure::image_wrapper wrapper{ m_device };

	std::vector<rhi::primitive::image> sc_images;
	wrapper.wrap_swapchainImages(swapchain, sc_images);

	storage::vault_writeAccessor<rhi::primitive::image> write{ m_swapchainImages };
	for (const rhi::primitive::image& i : sc_images) {
		write.create(i);
	}
}