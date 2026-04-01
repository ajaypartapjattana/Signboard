#pragma once

#include "Signboard/Resources/core/rsrc_buffers.h"

constexpr size_t STAGING_SIZE = 4ull * 1024 * 1024;

class rsrc_uploader {
public:
	rsrc_uploader(rsrc_buffers& buffers) noexcept;

	void* allocate(size_t size, size_t alignment, size_t& outOffest);

private:
	rsrc_buffers& r_buffers;

	uint32_t m_stagingBufferIndex;
	void* m_mappedStaging;

	size_t currentOffset;

};