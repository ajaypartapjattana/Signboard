#pragma once

#include "Signboard/Core/Containers/storage/storage.h"
#include "Signboard/RHI/rhi.h"

class res_images {
public:
	res_images(const rhi::core::device& device, const rhi::core::allocator& allocator);

	void create_image();
	void wrap_swapchainImages(const rhi::core::swapchain& swapchain);
private:


private:
	const rhi::core::device& m_device;
	const rhi::core::allocator& m_allocator;

	storage::vault<rhi::primitive::image> m_attachmentImages;
	storage::vault<rhi::primitive::image> m_swapchainImages;
	storage::vault<rhi::primitive::image> m_textureImages;

};