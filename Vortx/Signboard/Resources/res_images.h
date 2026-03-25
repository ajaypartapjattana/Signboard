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
	const rhi::creDevice& r_device;
	const rhi::creAllocator& r_allocator;

	storage::vault<rhi::pmvImage> m_attachmentImages;
	storage::vault<rhi::pmvImage> m_textureImages;

};