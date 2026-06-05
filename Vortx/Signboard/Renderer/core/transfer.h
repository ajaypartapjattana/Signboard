#pragma once

#include "Signboard/RHI/detail/core_t/resource_pool_t.h"
#include "Signboard/RHI/primitive/buffer.h"
#include "Signboard/core/Interfaces/renderer/upload/uploadINF.h"
#include <utility>

namespace rndr {

	constexpr size_t DEFAULT_STAGING_SIZE = 4ull * 1024 * 1024;

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

		void resetBuffer(size_t index) noexcept;

	public:
		TransferStage() = default;
		TransferStage(VkDevice device, VmaAllocator allocator) noexcept {
			allocateStagingBuffers(device, allocator, DEFAULT_STAGING_SIZE);
		}
		TransferStage(const TransferStage&) = delete;
		TransferStage(TransferStage&& other) noexcept
			:
			stagingBuffers(std::move(other.stagingBuffers)),
			stagingBufferInfos(std::move(other.stagingBufferInfos)),
			stagingBufferStates(std::move(other.stagingBufferStates)),
			freeBufferHint(other.freeBufferHint),
			r_device(std::exchange(other.r_device, VK_NULL_HANDLE)),
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

		~TransferStage() noexcept = default;

		VkResult allocateStagingBuffers(VkDevice device, VmaAllocator allocator, size_t size = DEFAULT_STAGING_SIZE, uint32_t count = 2) noexcept;

		VkResult stageUpload(const UploadSpan& src, const UploadTarget& dst);
		VkResult recordUploads(VkCommandBuffer commandBuffer, VkFence fence) noexcept;
		void informSubmissionFailure(VkFence fence) noexcept;
		void informTransferSuccess(VkFence fence) noexcept;

	};

}
