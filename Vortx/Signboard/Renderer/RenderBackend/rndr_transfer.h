#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Resources/resources.h"

constexpr size_t STAGING_SIZE = 4ull * 1024 * 1024;

class RHIContext;

class rndr_transfer {
public:
	rndr_transfer(const RHIContext& context, resourceView& resourceRead) noexcept;

	void* allocate(size_t size, size_t alignment, size_t& outOffest);
	VkResult recordUpload(const rhi::pmvBuffer& targetBuffer, const rhi::pmvCommandBuffer& cmd) const;

	void reset() noexcept;

private:
	rhi::pmvBuffer m_staging;
	void* m_mappedStaging;

	size_t currentOffset;

	const resourceView& m_resourceRead;

};