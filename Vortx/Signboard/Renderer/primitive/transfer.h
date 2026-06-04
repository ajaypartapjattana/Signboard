#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/core/core.h"
#include <utility>

constexpr size_t DEFAULT_STAGING_SIZE = 4ull * 1024 * 1024;

namespace rndr {

	class TransferStage {
	private:
		resource_pool<rhi::buffer> stagingBuffers;

		struct stagingBufferInfo {
			VkBuffer buffer;
			void* pMapped;
			size_t size;
		};
		std::vector<stagingBufferInfo> stagingBufferInfos;
		
		VkDevice r_device;
		VmaAllocator r_allocator;

		struct stagingBufferState{
			size_t currentOffset = 0;
			size_t flushStart = SIZE_MAX;
			size_t flushEnd = 0;

			struct CopyRegion {
				VkBuffer dstBuffer;
				VkBufferCopy copyInfo;
			};
			std::vector<CopyRegion> regions;
			VkFence fence;
		};
		std::vector<stagingBufferState> stagingBufferStates;
		size_t freeBufferHint = 0;

		struct AllocResult {
			size_t stagingIndex;
			void* pAllocBase;
			size_t offset;
		};

	public:
		TransferStage() = default;
		TransferStage(VkDevice device, VmaAllocator allocator) noexcept 
			:
			r_device(device)
		{
			allocateStagingBuffers(allocator, DEFAULT_STAGING_SIZE);
		}
		TransferStage(const TransferStage&) = delete;
		TransferStage(TransferStage&& other) noexcept
			:
			stagingBuffers(std::move(other.stagingBuffers)),
			stagingBufferInfos(std::move(other.stagingBufferInfos)),
			stagingBufferStates(std::move(other.stagingBufferStates)),
			freeBufferHint(other.freeBufferHint),
			r_allocator(std::exchange(other.r_allocator, VK_NULL_HANDLE))
		{

		}

		TransferStage& operator=(const TransferStage&) = delete;
		TransferStage& operator=(TransferStage&& other) noexcept {
			if (this == &other)
				return *this;

			stagingBuffers = std::move(other.stagingBuffers);
			stagingBufferInfos = std::move(other.stagingBufferInfos);
			freeBufferHint = other.freeBufferHint;
			r_allocator = std::exchange(other.r_allocator, VK_NULL_HANDLE);
			
			return *this;
		}

		~TransferStage() noexcept;

		VkResult allocateStagingBuffers(VmaAllocator allocator, size_t size, uint32_t count = 2) noexcept;

		VkResult stageUpload(const UploadSpan& src, const UploadTarget& dst);
		VkResult recordUploads(VkCommandBuffer commandBuffer, VkFence fence) noexcept;

		void resetBuffer(size_t index) noexcept;

	};

}
