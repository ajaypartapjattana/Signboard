#include "transfer.h"

namespace rndr {

	constexpr size_t UNIQUE_BUFFERS_PER_JOB = 8;

	VkResult TransferStage::root(VkDevice _Device, VmaAllocator _Allocator, const TransferStageCreateInfo* const pCreateInfo) noexcept {
		VkResult result = VK_ERROR_INITIALIZATION_FAILED;

		mem::stack _stack;

		VkCommandPool _commandPool = VK_NULL_HANDLE;
		mem::span<VkCommandBuffer> _commandBuffers;

		mem::span<VkFence> _transferFences;
		mem::span<uint8_t*> _transferOffsets;

		VkBuffer _stage = VK_NULL_HANDLE;
		VmaAllocation _allocation = VK_NULL_HANDLE;
		mem::span<uint8_t> _StageSpan;

		do {
			try {
				_stack.resize(size_t(4) << 10);
			}
			catch (const std::exception& _Except) {
				return VK_ERROR_INITIALIZATION_FAILED;
			}

			{
				VkCommandPoolCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
				createInfo.queueFamilyIndex = pCreateInfo->transferFamilyIndex;

				result = vkCreateCommandPool(_Device, &createInfo, nullptr, &_commandPool);
			}

			if (result != VK_SUCCESS)
				break;

			_commandBuffers = _stack.alloc<VkCommandBuffer>(pCreateInfo->maxConcurrentTransferJobs);

			{
				VkCommandBufferAllocateInfo allocateInfo{};
				allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocateInfo.pNext = nullptr;
				allocateInfo.commandPool = _commandPool;
				allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocateInfo.commandBufferCount = pCreateInfo->maxConcurrentTransferJobs;

				result = vkAllocateCommandBuffers(_Device, &allocateInfo, _commandBuffers.data());
			}

			if (result != VK_SUCCESS)
				break;

			_transferFences = _stack.alloc<VkFence>(pCreateInfo->maxConcurrentTransferJobs);

			{
				VkFenceCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

				for (uint32_t i = 0; i < pCreateInfo->maxConcurrentTransferJobs; ++i) {
					result = vkCreateFence(_Device, &createInfo, nullptr, &_transferFences[i]);
					if (result != VK_SUCCESS)
						break;
				}
			}

			if (result != VK_SUCCESS)
				break;

			_transferOffsets = _stack.alloc<uint8_t*>(pCreateInfo->maxConcurrentTransferJobs);

			{
				VkBufferCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				createInfo.size = pCreateInfo->stagingMemoryBudget;
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

				VmaAllocationInfo allocationInfo;

				result = vmaCreateBuffer(_Allocator, &createInfo, &allocationCreateInfo, &_stage, &_allocation, &allocationInfo);

				if (result != VK_SUCCESS)
					break;

				_StageSpan = mem::span{ static_cast<uint8_t*>(allocationInfo.pMappedData), allocationInfo.size };
			}

			const size_t jobCount = transferFences.size();

			if (jobCount)
				vkWaitForFences(r_device, jobCount, transferFences.data(), VK_TRUE, UINT64_MAX);

			if (stage)
				vmaDestroyBuffer(r_allocator, stage, allocation);

			for (size_t i = 0; i < jobCount; ++i)
				vkDestroyFence(r_device, transferFences[i], nullptr);

			if (commandPool)
				vkDestroyCommandPool(r_device, commandPool, nullptr);

			coreStack = std::move(_stack);

			commandBuffers = _commandBuffers;
			commandPool = _commandPool;

			transferFences = _transferFences;
			transferMarks = _transferOffsets;

			stage = _stage;
			allocation = _allocation;
			stageSpan = _StageSpan;

			pAllocHead =_StageSpan.pBegin;

			transferFamilyIndex = pCreateInfo->transferFamilyIndex;
			transferQueue = pCreateInfo->transferQueue;

			r_device = _Device;
			r_allocator = _Allocator;

			return VK_SUCCESS;

		} while (false);

		if (_stage)
			vmaDestroyBuffer(_Allocator, _stage, _allocation);

		const size_t jobCount = _commandBuffers.size();

		for (uint32_t i = 0; i < jobCount; ++i) {
			if (_transferFences[i])
				vkDestroyFence(_Device, _transferFences[i], nullptr);
		}

		if (_commandPool)
			vkDestroyCommandPool(_Device, _commandPool, nullptr);

		return result;
	}

	void TransferStage::reset() noexcept {
		const size_t jobCount = transferFences.size();
		
		if (jobCount)
			vkWaitForFences(r_device, jobCount, transferFences.data(), VK_TRUE, UINT64_MAX);

		if (stage)
			vmaDestroyBuffer(r_allocator, stage, allocation);

		if (commandPool)
			vkDestroyCommandPool(r_device, commandPool, nullptr);
	}

	VkResult TransferStage::reclaimJob(size_t _JobIndex) noexcept {
		VkResult result;

		result = vkGetFenceStatus(r_device, transferFences[_JobIndex]);

		switch (result) {
		case VK_SUCCESS:
			pAllocTail = transferMarks[_JobIndex];

			return VK_SUCCESS;

		case VK_NOT_READY:
			stalls++;

			return VK_NOT_READY;

		default:
			return result;
		}
	}

	VkResult TransferStage::beginStream() noexcept {
		VkResult result = reclaimJob(JobHint);

		if (result != VK_SUCCESS)
			return result;

		result = vkResetFences(r_device, 1, &transferFences[JobHint]);

		if (result != VK_SUCCESS)
			return result;

		result = vkResetCommandBuffer(commandBuffers[JobHint], 0);

		if (result != VK_SUCCESS)
			return result;

		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = nullptr;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			beginInfo.pInheritanceInfo = nullptr;

			result = vkBeginCommandBuffer(commandBuffers[JobHint], &beginInfo);
		}

		if (result != VK_SUCCESS)
			return result;

		recorder = JobHint;

		JobHint++;
		JobHint = JobHint != transferFences.size() ? JobHint : 0;

		pAllocBase = pAllocHead;
		reclaimHint = JobHint;

		return VK_SUCCESS;
	}

	VkResult TransferStage::commitChunk(const DataSource* const pSource, const size_t* const pGranularities, const size_t granularityCount, StageChunk* const pChunk) noexcept {
		assert(pSource && pGranularities && granularityCount && pChunk);

		const size_t availableSize = pAllocHead >= pAllocTail ? static_cast<size_t>(stageSpan.pEnd - pAllocHead) : static_cast<size_t>(pAllocTail - pAllocHead);

		if (pSource->size > availableSize) {
			if(pAllocHead < pAllocTail)
				return VK_NOT_READY;

			size_t transferSize, granularityIndex = 0;

			for (const size_t* pGranularity = pGranularities + granularityCount; pGranularity != pGranularities; --pGranularity) {
				transferSize = mem::alignDown(availableSize, *pGranularity);
				
				if (!transferSize)
					continue;

				granularityIndex = static_cast<size_t>(pGranularity - pGranularities);
				break;
			}

			if (transferSize) {
				memcpy(pAllocHead, pSource->pData, transferSize);

				pChunk->offset = static_cast<VkDeviceSize>(pAllocHead - stageSpan.pBegin);
				pChunk->size = static_cast<VkDeviceSize>(transferSize);
				pChunk->granularityIndex = granularityIndex;

				pAllocHead += transferSize;

				return VK_INCOMPLETE;
			}

			if (pSource->size > static_cast<size_t>(pAllocTail - stageSpan.pBegin))
				return VK_NOT_READY;

			pAllocHead = stageSpan.pBegin;
		}

		memcpy(pAllocHead, pSource->pData, pSource->size);

		pChunk->offset = static_cast<VkDeviceSize>(pAllocHead - stageSpan.pBegin);
		pChunk->size = static_cast<VkDeviceSize>(pSource->size);
		pChunk->granularityIndex = 0ull;

		pAllocHead += pSource->size;

		return VK_SUCCESS;
	}

	VkResult TransferStage::streamBufferUpload(const DataSource* const pSource, VkBuffer _DstBuffer, VkDeviceSize* const pOffset) noexcept {
		assert(pSource && _DstBuffer && pOffset);

		VkResult result;

		StageChunk chunk;

		if (!pSource->pData || !pSource->size)
			return VK_SUCCESS;

		constexpr size_t bufferGranularity = 1ull;

		const size_t fenceCount = transferFences.size();

		while (true) {
			DataSource source = { reinterpret_cast<const uint8_t*>(pSource->pData) + *pOffset, pSource->size - *pOffset };

			result = commitChunk(&source, &bufferGranularity, 1, &chunk);

			switch (result) {
			case VK_SUCCESS:
			case VK_INCOMPLETE:
				VkBufferCopy region = { chunk.offset, *pOffset, chunk.size };

				vkCmdCopyBuffer(commandBuffers[recorder], stage, _DstBuffer, 1, &region);

				*pOffset += chunk.size;
				
				if (result == VK_SUCCESS)
					return VK_SUCCESS;

				continue;

			case VK_NOT_READY:
				result = reclaimJob(reclaimHint);

				if (result != VK_SUCCESS)
					return result;

				reclaimHint++;
				reclaimHint = reclaimHint != fenceCount ? reclaimHint : 0;

				continue;

			default:
				return result;

			}
		}
	}

	VkResult TransferStage::streamImageUpload(const DataSource* const pSource, VkImage _DstImage, const TransferImageAttributes* const pImageInfo, VkOffset3D* const pOffset) {
		assert(pSource && _DstImage && pImageInfo);

		VkResult result;

		StageChunk chunk;

		if (!pSource->size)
			return VK_SUCCESS;

		const size_t fenceCount = transferFences.size();

		size_t granularity[3];
		granularity[0] = pImageInfo->texelSize;
		granularity[1] = pImageInfo->extent.width * granularity[0];
		granularity[2] = pImageInfo->extent.height * granularity[1];

		while (true) {
			const size_t sourceOffset = pOffset->z * granularity[2] + pOffset->y * granularity[1] + pOffset->x * granularity[0];

			size_t pending, granularityLimit;

			if (pOffset->x) {
				pending = granularity[0] * ((size_t)pImageInfo->extent.width - pOffset->x);
				granularityLimit = 1;
			}
			else if (pOffset->y) {
				pending = granularity[1] * ((size_t)pImageInfo->extent.height - pOffset->y);
				granularityLimit = 2;
			}
			else {
				pending = granularity[2] * ((size_t)pImageInfo->extent.depth - pOffset->z);
				granularityLimit = 3;
			}

			DataSource source = { reinterpret_cast<const uint8_t*>(pSource->pData) + sourceOffset, pending };

			result = commitChunk(&source, granularity, granularityLimit, &chunk);

			switch (result) {
			case VK_SUCCESS: {
				VkBufferImageCopy region = { chunk.offset, 0, 0, pImageInfo->subresources, *pOffset, pImageInfo->extent };

				vkCmdCopyBufferToImage(commandBuffers[recorder], stage, _DstImage, pImageInfo->layout, 1, &region);
			}

				*pOffset = { 0, 0, 1 };

				return VK_SUCCESS;

			case VK_INCOMPLETE: {
				VkExtent3D transferExtent = { pImageInfo->extent.width - pOffset->x, pImageInfo->extent.height - pOffset->y, pImageInfo->extent.depth - pOffset->z };

				const uint32_t units = static_cast<uint32_t>(chunk.size / granularity[chunk.granularityIndex]);

				switch (chunk.granularityIndex) {
				case 0:
					transferExtent.width = units;
					transferExtent.height = 1;
					transferExtent.depth = 1;

					break;
					
				case 1:
					transferExtent.height = units;
					transferExtent.depth = 1;

					break;

				case 2:
					transferExtent.depth = units;

					break;
				}

				VkBufferImageCopy region = { chunk.offset, 0, 0, pImageInfo->subresources, *pOffset, transferExtent };

				vkCmdCopyBufferToImage(commandBuffers[recorder], stage, _DstImage, pImageInfo->layout, 1, &region);
				
				switch (chunk.granularityIndex) {
				case 0:
					pOffset->x += transferExtent.width;
					if (pOffset->x == pImageInfo->extent.width) {
						pOffset->x = 0;
						pOffset->y++;
					}

					break;

				case 1:
					pOffset->y += transferExtent.height;
					if (pOffset->y == pImageInfo->extent.height) {
						pOffset->y = 0;
						pOffset->z++;
					}

					break;

				case 2:
					pOffset->z += transferExtent.depth;

					break;
				}
			}

				if (result == VK_SUCCESS)
					return VK_SUCCESS;

				continue;

			case VK_NOT_READY:
				result = reclaimJob(reclaimHint);

				if (result != VK_SUCCESS)
					return result;

				reclaimHint++;
				reclaimHint = reclaimHint != fenceCount ? reclaimHint : 0;

				continue;

			default:
				return result;

			}
		}
	}

	VkResult TransferStage::flushStream(VkBool32 _WaitTransfers, VkSemaphore _Signal) noexcept {
		VkResult result;

		result = vkEndCommandBuffer(commandBuffers[recorder]);

		if (result != VK_SUCCESS)
			return result;

		const VkDeviceSize flushBase = static_cast<VkDeviceSize>(pAllocBase - stageSpan.pBegin);
		const VkDeviceSize flushSize = pAllocHead > pAllocBase ? static_cast<VkDeviceSize>(pAllocHead - pAllocBase) : static_cast<VkDeviceSize>(stageSpan.pEnd - pAllocBase);

		result = vmaFlushAllocation(r_allocator, allocation, flushBase, flushSize);

		if (result != VK_SUCCESS)
			return result;

		if (pAllocHead < pAllocBase) {
			result = vmaFlushAllocation(r_allocator, allocation, 0, static_cast<VkDeviceSize>(pAllocHead - stageSpan.pBegin));

			if (result != VK_SUCCESS)
				return result;
		}

		{
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pNext = nullptr;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffers[recorder];
			submitInfo.signalSemaphoreCount = _Signal ? 1 : 0;
			submitInfo.pSignalSemaphores = _Signal ? &_Signal : nullptr;

			result = vkQueueSubmit(transferQueue, 1, &submitInfo, transferFences[recorder]);
		}

		if (result != VK_SUCCESS)
			return result;

		transferMarks[recorder] = pAllocHead;

		stats.current = (pAllocHead >= pAllocTail) ? static_cast<size_t>(pAllocHead - pAllocTail) : static_cast<size_t>(stageSpan.pEnd - pAllocTail) + static_cast<size_t>(pAllocHead - stageSpan.pBegin);
		stats.peak = stats.current > stats.peak ? stats.current : stats.peak;

		return VK_SUCCESS;
	}

}
