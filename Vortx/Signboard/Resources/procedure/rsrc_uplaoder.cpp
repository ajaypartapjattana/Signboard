#include "rsrc_uploader.h"

rsrc_uploader::rsrc_uploader(rsrc_buffers& buffers) noexcept
	:
	r_buffers(buffers)
{
	rsrc_buffers::createInfo _staginfo{};
	_staginfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	_staginfo.memory = VMA_MEMORY_USAGE_AUTO;
	_staginfo.allocationFlags = 
		VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
		VMA_ALLOCATION_CREATE_MAPPED_BIT;
	_staginfo.size = STAGING_SIZE;

	m_stagingBufferIndex = r_buffers.createBuffer(_staginfo);
	m_mappedStaging = r_buffers.getbufferMapping(m_stagingBufferIndex);
}

void* rsrc_uploader::allocate(size_t size, size_t alignment, size_t& outOffest) {
	size_t alignedOffest = (currentOffset + alignment - 1) & ~(alignment - 1);

	if (alignedOffest + size > STAGING_SIZE) {
		return nullptr;
	}

	outOffest = alignedOffest;
	currentOffset = alignedOffest + size;

	return static_cast<char*>(m_mappedStaging) + outOffest;
}
