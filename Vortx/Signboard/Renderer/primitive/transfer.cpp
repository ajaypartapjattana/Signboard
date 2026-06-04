#include "transfer.h"

namespace rndr {

	VkResult TransferStage::allocateStagingBuffers(VmaAllocator allocator, size_t size, uint32_t count) noexcept {
		resource_pool<rhi::buffer> buffers{ allocator };
		std::vector<stagingBufferInfo> bufferInfos(count);

		{
			VkBufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.size = size;
			createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo allocationCreateInfo{};
			allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
			allocationCreateInfo.requiredFlags = 0;
			allocationCreateInfo.preferredFlags = 0;
			allocationCreateInfo.memoryTypeBits = 0;
			allocationCreateInfo.pool = VK_NULL_HANDLE;
			allocationCreateInfo.pUserData = nullptr;
			allocationCreateInfo.priority = 0.0f;

			size_t index;
			VmaAllocationInfo allocationInfo{};
			
			for (uint32_t i = 0; i < count; ++i) {
				VkResult result = buffers.create(&createInfo, &allocationCreateInfo, &index, &allocationInfo);
				if (result != VK_SUCCESS)
					return result;

				bufferInfos[i] = { buffers[index], allocationInfo.pMappedData, allocationInfo.size };
			}
			
		}

		stagingBuffers = std::move(buffers);
		stagingBufferInfos = std::move(bufferInfos);

		stagingBufferStates.clear();
		stagingBufferStates.resize(count);

		r_allocator = allocator;

		return VK_SUCCESS;
	}

	VkResult TransferStage::stageUpload(const UploadSpan& src, const UploadTarget& dst) {
		if (!src.valid())
			return VK_ERROR_INVALID_EXTERNAL_HANDLE;

		const size_t bufferCount = stagingBufferStates.size();

		size_t bufferIndex = SIZE_MAX;
		size_t offset = 0;

		for (size_t i = 0; i < bufferCount; ++i) {
			size_t index = (freeBufferHint + i) % bufferCount;

			stagingBufferState& bufferState = stagingBufferStates[index];

			if (bufferState.fence) {
				VkResult result = vkGetFenceStatus(r_device, bufferState.fence);

				if (result == VK_NOT_READY)
					continue;

				resetBuffer(index);
			}

			const stagingBufferInfo& bufferInfo = stagingBufferInfos[index];

			size_t alignedOffset = (bufferState.currentOffset + src.alignment - 1) & ~(src.alignment - 1);

			if (alignedOffset + src.size > bufferInfo.size)
				continue;

			bufferIndex = index;
			offset = alignedOffset;

			break;
		}

		if (bufferIndex == SIZE_MAX)
			return VK_INCOMPLETE;

		const stagingBufferInfo& bufferInfo = stagingBufferInfos[bufferIndex];
		stagingBufferState& bufferState = stagingBufferStates[bufferIndex];

		void* pDst = static_cast<char*>(bufferInfo.pMapped) + offset;

		memcpy(pDst, src.data, src.size);

		bufferState.currentOffset = offset + src.size;
		bufferState.flushStart = std::min(bufferState.flushStart, offset);
		bufferState.flushEnd = std::max(bufferState.flushEnd, offset + src.size);

		bufferState.regions.push_back({ dst.buffer, { offset, dst.offset, static_cast<VkDeviceSize>(src.size) } });

		freeBufferHint = bufferIndex;

		return VK_SUCCESS;
	}

	void TransferStage::resetBuffer(size_t index) noexcept {
		stagingBufferState& bufferState = stagingBufferStates[index];

		bufferState.currentOffset = 0;
		bufferState.flushStart = SIZE_MAX;
		bufferState.flushEnd = 0;
		bufferState.fence = VK_NULL_HANDLE;
	}

	VkResult TransferStage::recordUploads(VkCommandBuffer commandBuffer, VkFence fence) noexcept {
		const size_t bufferCount = stagingBufferStates.size();

		for (size_t i = 0; i < bufferCount; ++i) {
			stagingBufferState& bufferState = stagingBufferStates[i];

			if (bufferState.regions.empty())
				continue;

			std::unordered_map<VkBuffer, std::vector<VkBufferCopy>> batches;

			for (const auto& r : bufferState.regions) {
				batches[r.dstBuffer].push_back(r.copyInfo);
			}

			VkBuffer stagingBuffer = stagingBufferInfos[i].buffer;

			size_t flushSize = bufferState.flushEnd - bufferState.flushStart;
			vmaFlushAllocation(r_allocator, stagingBuffers.allocation(i), bufferState.flushStart, flushSize);

			for (auto& [dstBuffer, copyRegionInfo] : batches) {
				uint32_t copyRegionCount = static_cast<uint32_t>(copyRegionInfo.size());
				vkCmdCopyBuffer(commandBuffer, stagingBuffer, dstBuffer, copyRegionCount, copyRegionInfo.data());
			}

			bufferState.fence = fence;

			bufferState.regions.clear();
		}

		return VK_SUCCESS;
	}

}
