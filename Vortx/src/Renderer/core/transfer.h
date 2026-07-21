#pragma once

#include "src/Core/Memory/memory.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <vector>
#include <utility>
#include <cstddef>

namespace rndr {

	struct DataSource {
		const void* pData;
		size_t size;
	};

	struct TransferImageAttributes {
		VkImageLayout layout;
		VkImageSubresourceLayers subresources;
		VkExtent3D extent;
		uint32_t texelSize;
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
		size_t JobHint = 0;

		mem::span<uint8_t*> transferMarks;
		size_t reclaimHint = 0;

		VkBuffer stage = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;
		mem::span<uint8_t> stageSpan;

		uint8_t* pAllocBase = nullptr;
		size_t recorder = 0;

		uint8_t* pAllocTail = nullptr;
		uint8_t* pAllocHead = nullptr;

		struct {
			size_t current;
			size_t peak = 0;
		} stats;
		
		uint32_t stalls = 0;

		uint32_t transferFamilyIndex;
		VkQueue transferQueue;
	
		VkDevice r_device = VK_NULL_HANDLE;
		VmaAllocator r_allocator = VK_NULL_HANDLE;

		VkResult reclaimJob(size_t _JobIndex) noexcept;

		struct StageChunk {
			VkDeviceSize offset;
			VkDeviceSize size;
			size_t granularityIndex;
		};

		VkResult commitChunk(const DataSource* const pSource, const size_t* const pGranularities, const size_t granularityCount, StageChunk* const pChunk) noexcept;

	public:
		TransferStage() = default;
		TransferStage(const TransferStage&) = delete;

		TransferStage& operator=(const TransferStage&) = delete;

		~TransferStage() noexcept = default;

		VkResult root(VkDevice _Device, VmaAllocator _Allocator, const TransferStageCreateInfo* const pCreateInfo) noexcept;

		VkResult beginStream() noexcept;

		VkResult streamBufferUpload(const DataSource* const pSource, VkBuffer _DstBuffer, VkDeviceSize* const pOffset) noexcept;
		VkResult streamImageUpload(const DataSource* const pSource, VkImage _DstImage, const TransferImageAttributes* const pImageInfo, VkOffset3D* const pOffset) noexcept;

		VkResult flushStream(VkBool32 _WaitTransfers, VkSemaphore _Signal) noexcept;

		void reset() noexcept;

	};

}
