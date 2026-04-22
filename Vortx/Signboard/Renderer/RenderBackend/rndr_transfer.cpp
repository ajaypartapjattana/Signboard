#include "rndr_transfer.h"

#include "Signboard/Renderer/Context/render_context_Access.h"

rndr_transfer::rndr_transfer(const RHIContext& context, const ResourceView& resources) noexcept
	:
	r_resources(resources)
{
	regions.reserve(10);

	rhi::pcdBufferAllocate prcdr{ rndr_context_Access::get_allocator(context) };

	prcdr.add_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	prcdr.prefer_memory(VMA_MEMORY_USAGE_AUTO);
	prcdr.set_allocationFlags(VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
	prcdr.size(STAGING_SIZE);

	prcdr.publish(m_staging);
	m_mappedStaging = m_staging.native_mapped();
}

VkResult rndr_transfer::stageUpload(const UploadSpan& src, const UploadTarget& dst) {
	if (!src.valid())
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	
	size_t srcOffset;
	void* _mpd = allocate(src.size, src.alignment, srcOffset);

	if (!_mpd) {
		return VK_INCOMPLETE;
	}

	memcpy(_mpd, src.data, src.size);

	flushStart = std::min(flushStart, srcOffset);
	flushEnd = std::max(flushEnd, srcOffset + src.size);

	regions.push_back({
		dst.buffer,
		VkBufferCopy{ srcOffset, dst.offset, static_cast<VkDeviceSize>(src.size) }
	});

	return VK_SUCCESS;
}

void* rndr_transfer::allocate(size_t size, size_t alignment, size_t& outOffest) {
	size_t alignedOffest = (currentOffset + alignment - 1) & ~(alignment - 1);

	if (alignedOffest + size > STAGING_SIZE) {
		return nullptr;
	}

	outOffest = alignedOffest;
	currentOffset = alignedOffest + size;

	return static_cast<char*>(m_mappedStaging) + outOffest;
}

VkResult rndr_transfer::recordUploads(const rhi::pmvCommandBuffer& CMDGraphics) const {
	if (regions.empty())
		return VK_INCOMPLETE;

	m_staging.flush(flushStart, flushEnd);

	rhi::pcdCommandBufferRecorder prcdr{ CMDGraphics };

	std::unordered_map<uint32_t, std::vector<VkBufferCopy>> batches;

	for (const CopyRegion& r : regions) {
		batches[r.targetBuffer].push_back(r.copyInfo);
	}

	for (auto& [buffers, copies] : batches) {
		prcdr.uploadBuffer(m_staging, *r_resources.buffers.get(buffers), copies);
	}

	prcdr.end_recording();

	return VK_SUCCESS;
}

void rndr_transfer::reset() noexcept {
	currentOffset = 0;
	regions.clear();
}
