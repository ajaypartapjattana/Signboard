#pragma once

#include <vulkan/vulkan.h>
#include <external/VMA/vma.h>
#include <vector>
#include <utility>

namespace rndr {

	struct TransferSizeInfo {
		const void* pData;
		size_t size;
		size_t alignment;
	};

	struct TargetBufferInfo {
		VkBuffer buffer;
		VkDeviceSize dstOffset;
	};

	struct ImageTransferInfo {
		VkImage image;

		VkImageSubresourceLayers subresources;
		VkExtent3D extent;
		VkOffset3D offset;
	};

	enum TransferMode {
		TRANSFER_MODE_IMMEDIATE,
		TRANSFER_MODE_BATCH,
		TRANSFER_MODE_STREAM
	};

	struct TransferStageCreateInfo {
		uint32_t transferFamilyIndex;
		VkQueue transferQueue;

		VkDeviceSize stagingMemoryBudget;
		VkDeviceSize preferredTransferSize;
		uint32_t sizeClassCount;
		float transferSizeDistribution;

		uint32_t maxConcurrentTransferJobs;
	};

	class TransferStage {
	private:
		VkDevice r_device = VK_NULL_HANDLE;
		VmaAllocator r_allocator = VK_NULL_HANDLE;
		
		uint32_t transferFamilyIndex;
		VkQueue transferQueue;

		VkCommandPool commandPool = VK_NULL_HANDLE;

		struct TransferJob {
			VkCommandBuffer commandBuffer;
			VkFence fence;
		};

		std::vector<TransferJob> jobs;

		struct Stage {
			VkBuffer buffer;
			VmaAllocation allocation;
			void* pMapped;

			VkDeviceSize currentOffset = 0;
			VkDeviceSize flushStart = SIZE_MAX;
			VkDeviceSize flushEnd = 0;
		};

		std::vector<Stage> stages;
		std::vector<VkFence> stageFences;
		std::vector<VkDeviceSize> availableStageSizes;
		
		std::vector<uint32_t> sizeClassRanges;
		uint32_t baseClassOffset;

		struct Allocation {
			VkBuffer buffer;
			VkDeviceSize offset;
			uint32_t size;
		};

		std::vector<Allocation> allocations;

		enum TransferType {
			TRANSFER_TYPE_BUFFER,
			TRANSFER_TYPE_IMAGE
		};

		struct BufferCopy {
			size_t allocationIndex;

			VkBuffer dstBuffer;
			VkBufferCopy copy;
		};
		
		struct ImageCopy {
			size_t allocationIndex;

			VkImage image;
			VkImageLayout dstLayout;
			VkImageCopy copy;
		};

		struct Copy {
			TransferType type;
			size_t allocationIndex;

			union {
				BufferCopy bufferCopyInfo;
				ImageCopy imageCopyInfo;
			};
		};

		std::vector<Copy> copyRegions;

		void resetBuffer(size_t index) noexcept;

	public:
		TransferStage() = default;
		TransferStage(const TransferStage&) = delete;
		TransferStage(TransferStage&& other) noexcept
			:
			stages(std::move(other.stages)),
			freeBufferHint(other.freeBufferHint),
			r_device(std::exchange(other.r_device, VK_NULL_HANDLE)),
			r_allocator(std::exchange(other.r_allocator, VK_NULL_HANDLE))
		{

		}

		TransferStage& operator=(const TransferStage&) = delete;
		TransferStage& operator=(TransferStage&& other) noexcept {
			if (this == &other)
				return *this;

			stages = std::move(other.stages);
			freeBufferHint = other.freeBufferHint;
			r_device = std::exchange(other.r_device, VK_NULL_HANDLE);
			r_allocator = std::exchange(other.r_allocator, VK_NULL_HANDLE);
			
			return *this;
		}

		~TransferStage() noexcept = default;

		VkResult root(VkDevice device, VmaAllocator allocator, const TransferStageCreateInfo* pCreateInfo) noexcept;

		VkResult stageBufferUpload(const TransferSizeInfo* pSizeInfos, const TargetBufferInfo* pTransferInfos, uint32_t infoCount, TransferMode mode);
		VkResult stageImageUpload(const void* pData, size_t size, size_t alignment, const VkImage target, const VkDeviceSize offset);

		VkResult submitUploads(VkCommandBuffer commandBuffer, VkFence fence) noexcept;

		void reset() noexcept;

	};

}
