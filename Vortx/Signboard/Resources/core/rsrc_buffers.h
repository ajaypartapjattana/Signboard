#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/core/Containers/storage.h"

class rsrc_buffers {
public:
	rsrc_buffers(const rhi::creAllocator& allocator);

	struct createInfo {
		VkBufferUsageFlags usage;
		VmaMemoryUsage memory;
		VmaAllocationCreateFlags allocationFlags;
		size_t size;
	};

	storage::storage_handle createBuffer(const createInfo& info) noexcept;
	void* getbufferMapping(storage::storage_handle handle) const;

	storage::vault_readAccessor<rhi::pmvBuffer> get_bufferReadAccess() const noexcept;

private:
	rhi::pcdBufferAllocator m_allocator;

	storage::vault<rhi::pmvBuffer> m_buffers;
	storage::vault_writeAccessor<rhi::pmvBuffer> m_writer;

};