#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Resources/resources.h"

#include "Signboard/Core/Interfaces/renderer/upload/uploadINF.h"

constexpr size_t STAGING_SIZE = 4ull * 1024 * 1024;

class RHIContext;

class rndr_transfer {
public:
	rndr_transfer(const RHIContext& context, resourceView& resourceRead) noexcept;

	VkResult stageUpload(const UploadSpan& src, const UploadTarget& dst);
	VkResult recordUploads(const rhi::pmvCommandBuffer& CMDGraphics) const;

	void reset() noexcept;

private:
	void* allocate(size_t size, size_t alignment, size_t& outOffest);

private:
	rhi::pmvBuffer m_staging;
	void* m_mappedStaging;

	size_t currentOffset;
	size_t flushStart = SIZE_MAX;
	size_t flushEnd = 0;

	struct CopyRegion {
		uint32_t targetBuffer;
		VkBufferCopy copyInfo;
	};
	std::vector<CopyRegion> regions;

	const resourceView& m_resourceRead;

};