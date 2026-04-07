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

	uint32_t createBuffer(const createInfo& info) noexcept;

	ctnr::vltView<rhi::pmvBuffer> read_buffers() const noexcept;

private:
	rhi::pcdBufferAllocator _allctr;

	ctnr::vault<rhi::pmvBuffer> m_buffers;
	ctnr::vault_writeAccessor<rhi::pmvBuffer> m_writer;

};