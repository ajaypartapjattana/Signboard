#pragma once

#include "Signboard/Core/Containers/storage/storage.h"
#include "Signboard/RHI/rhi.h"

class rndr_context;

class rsrc_images {
public:
	rsrc_images(const rndr_context& ctx);

	struct createInfo {
		VkFormat format;
		VkExtent3D extent;
		VkImageUsageFlags usage;
		VkImageAspectFlags aspect;
	};

	storage::storage_handle createImage(const createInfo& info);

	storage::vault_readAccessor<rhi::pmvImage> get_imageReadAccess() const noexcept;

private:
	const rhi::creDevice& r_device;
	const rhi::creAllocator& r_allocator;

	storage::vault<rhi::pmvImage> m_textureImages;
	storage::vault_writeAccessor<rhi::pmvImage> m_writer;

	storage::vault<rhi::pmvImage> m_attachmentImages;

};