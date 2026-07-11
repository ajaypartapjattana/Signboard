#pragma once

#include "src/Core/Memory/memory.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
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

	struct TargetImageInfo {
		VkImage image;

		VkImageSubresourceLayers subresources;
		VkExtent3D extent;
		VkOffset3D offset;
	};

	struct TransferStageCreateInfo {
		uint32_t transferFamilyIndex;
		VkQueue transferQueue;

		VkDeviceSize stagingMemoryBudget;
		uint32_t maxConcurrentTransferJobs;
	};

	class TransferStage {
	private:
		mem::stack coreStack;
		
		mem::span<VkCommandBuffer> commandBuffers;
		VkCommandPool commandPool = VK_NULL_HANDLE;

		mem::span<VkFence> transferFences;
		size_t acquireHint = 0;

		mem::span<VkDeviceSize> transferOffsets;
		size_t reclaimHint = 0;

		VkBuffer stage = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;
		VkDeviceSize bufferSize;
		void* pMapped;

		uint32_t transferFamilyIndex;
		VkQueue transferQueue;
	
		VkDevice r_device = VK_NULL_HANDLE;
		VmaAllocator r_allocator = VK_NULL_HANDLE;

		VkDeviceSize head = 0;
		VkDeviceSize tail = 0;

		struct {
			size_t current;
			size_t peak = 0;
			uint32_t stalls = 0;
		} stats;

		VkResult acquireTransferJob(size_t* pIndex) noexcept;

		mem::span<uint8_t> allocate(const size_t _Size, const size_t _Align) noexcept;

	public:
		TransferStage() = default;
		TransferStage(const TransferStage&) = delete;

		TransferStage& operator=(const TransferStage&) = delete;

		~TransferStage() noexcept = default;

		VkResult root(VkDevice _Device, VmaAllocator _Allocator, const TransferStageCreateInfo* pCreateInfo) noexcept;

		VkResult stageBufferUpload(const TransferSizeInfo* pSizeInfos, const TargetBufferInfo* pTransferInfos, uint32_t _TransferCount, VkSemaphore _Signal);
		VkResult stageImageUpload(const void* pData, size_t size, size_t alignment, const VkImage target, const VkDeviceSize offset);

		VkResult submitUploads(VkCommandBuffer commandBuffer, VkFence fence) noexcept;

		void reset() noexcept;

	};

}
