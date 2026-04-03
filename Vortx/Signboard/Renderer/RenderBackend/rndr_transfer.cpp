#include "rndr_transfer.h"

#include "Signboard/Renderer/Context/render_context_Access.h"

rndr_transfer::rndr_transfer(const RHIContext& context, resourceView& resourceRead) noexcept
	:
	currentOffset(0),

	m_resourceRead(std::move(resourceRead))
{
	rhi::pcdBufferAllocator prcdr{ rndr_context_Access::get_allocator(context) };

	prcdr.addUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	prcdr.setMemoryPreference(VMA_MEMORY_USAGE_AUTO);
	prcdr.setMemoryFlags(VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
	prcdr.setBufferSize(STAGING_SIZE);

	prcdr.allocateBuffer(m_staging);
	m_mappedStaging = prcdr.mapBuffer(m_staging);
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

VkResult rndr_transfer::recordUpload(const rhi::pmvBuffer& tgt, const rhi::pmvCommandBuffer& cmd) const {
	rhi::pcdCommandBufferRecorder prcdr{ cmd };

	VkBufferCopy copy{};
	copy.dstOffset = 0;
	copy.size = currentOffset;
	copy.srcOffset = 0;

	std::vector<VkBufferCopy> copies = { copy };

	prcdr.uploadBuffer(m_staging, tgt, copies);
}

void rndr_transfer::reset() noexcept {
	currentOffset = 0;
}
