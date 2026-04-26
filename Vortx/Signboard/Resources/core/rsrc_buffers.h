#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/core/Containers/storage.h"

class rsrc_buffers {
public:
	rsrc_buffers(const rhi::creAllocator& allocator, ctnr::vltWrite<rhi::pmvBuffer>&& buffer_write);

	struct createInfo {
		VkBufferUsageFlags usage;
		VmaMemoryUsage memory;
		VmaAllocationCreateFlags allocationFlags;
		size_t size;
	};

	uint32_t createVertexBuffer(size_t size) noexcept;
	uint32_t createIndexBuffer(size_t size) noexcept;

	_NODISCARD uint32_t createBuffer(const createInfo& info) noexcept;

private:
	rhi::pcdBufferAllocate r_bufferAllocator;
	ctnr::vltWrite<rhi::pmvBuffer> m_writer;

};