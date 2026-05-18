#pragma once

#include "Signboard/Renderer/Internal/rndr_pInterface.h"

#include "Signboard/RHI/rhi.h"
#include "Signboard/core/core.h"

constexpr size_t STAGING_SIZE = 4ull * 1024 * 1024;

namespace rndr {

	class transferControl {
	public:
		transferControl(const rhi::creAllocator& allocator) noexcept;

		void bindResources(const resourceView& resources) noexcept;

		VkResult stageUpload(const UploadSpan& src, const UploadTarget& dst);
		VkResult recordUploads(const rhi::pmvCommandBuffer& CMDGraphics) const noexcept;

		void reset() noexcept;

	private:
		void* allocate(size_t size, size_t alignment, size_t& outOffest);

	private:
		const resourceView* r_resources;

		rhi::pmvBuffer m_staging;
		void* m_mappedStaging;

		size_t currentOffset = 0;
		size_t flushStart = SIZE_MAX;
		size_t flushEnd = 0;

		struct CopyRegion {
			uint32_t targetBuffer;
			VkBufferCopy copyInfo;
		};
		std::vector<CopyRegion> regions;

	};

}
