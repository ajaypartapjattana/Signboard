#pragma once

#include "Signboard/Core/Containers/storage/storage.h"
#include "Signboard/RHI/rhi.h"

class rndr_context;

class res_images {
public:
	res_images(const rndr_context& ctx);

	void create_image();
private:


private:
	const rhi::core::device& r_device;
	const rhi::core::allocator& r_allocator;

	storage::vault<rhi::primitive::image> m_attachmentImages;
	storage::vault<rhi::primitive::image> m_textureImages;

};