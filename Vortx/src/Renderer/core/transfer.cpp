#include "transfer.h"

#include <array>
#include <unordered_map>

namespace rndr {

	VkResult TransferStage::root(VkDevice _Device, VmaAllocator _Allocator, const TransferStageCreateInfo* pCreateInfo) noexcept {
		VkResult result{};

		mem::stack _stack;

		VkCommandPool _commandPool = VK_NULL_HANDLE;
		mem::span<VkCommandBuffer> _commandBuffers;

		mem::span<VkFence> _transferFences;
		mem::span<VkDeviceSize> _transferOffsets;

		VkBuffer _stage = VK_NULL_HANDLE;
		VmaAllocation _allocation = VK_NULL_HANDLE;
		VkDeviceSize _allocSize;
		void* _pMapped;

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

			_transferOffsets = _stack.alloc<VkDeviceSize>((VkDeviceSize)pCreateInfo->maxConcurrentTransferJobs);
			_transferOffsets.defaultConstruct();

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

				_allocSize = allocationInfo.size;
				_pMapped = allocationInfo.pMappedData;
			}

		} while (false);

		if(result == VK_SUCCESS) {
			const size_t jobCount = commandBuffers.size();

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
			transferOffsets = _transferOffsets;

			stage = _stage;
			allocation = _allocation;
			bufferSize = _allocSize;
			pMapped = _pMapped;

			transferFamilyIndex = pCreateInfo->transferFamilyIndex;
			transferQueue = pCreateInfo->transferQueue;

			r_device = _Device;
			r_allocator = _Allocator;

			return VK_SUCCESS;
		}

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

	VkResult TransferStage::acquireTransferJob(size_t* pIndex) noexcept {
		VkResult result;

		result = vkGetFenceStatus(r_device, transferFences[acquireHint]);
		switch (result) {
		case VK_SUCCESS:

			break;

		case VK_NOT_READY:
			stats.stalls++;

			result = vkWaitForFences(r_device, 1, &transferFences[acquireHint], VK_TRUE, UINT64_MAX);

			if (result != VK_SUCCESS)
				return result;

			break;

		default:
			return result;

		}

		result = vkResetFences(r_device, 1, &transferFences[acquireHint]);

		if (result != VK_SUCCESS)
			return result;

		result = vkResetCommandBuffer(commandBuffers[acquireHint], 0);

		if (result != VK_SUCCESS)
			return result;

		*pIndex = acquireHint;

		acquireHint++;
		acquireHint %= transferFences.size();

		return VK_SUCCESS;
	}

	mem::span<uint8_t> TransferStage::allocate(const size_t _Size, const size_t _Align) noexcept {
		if (_Size > bufferSize)
			return {};
		
		VkDeviceSize baseOff = mem::alignUp(head, _Align);
		VkDeviceSize endOff = baseOff + _Size;

		if (head >= tail) {
			if (endOff <= bufferSize) {
				head = endOff;

				return { static_cast<uint8_t*>(pMapped) + baseOff , static_cast<uint8_t*>(pMapped) + endOff };
			}

			baseOff = 0;
			endOff = _Size;
		}
		
		if (endOff <= tail) {
			head = endOff;

			return { static_cast<uint8_t*>(pMapped) + baseOff , static_cast<uint8_t*>(pMapped) + endOff };
		}

		const size_t fenceCount = transferFences.size();

		while (endOff > tail) {
			VkResult result = vkGetFenceStatus(r_device, transferFences[reclaimHint]);
			switch (result) {
			case VK_SUCCESS:

				break;

			case VK_NOT_READY:
				stats.stalls++;

				result = vkWaitForFences(r_device, 1, &transferFences[reclaimHint], VK_TRUE, UINT32_MAX);

				if (result != VK_SUCCESS)
					return {};

				break;

			default:
				return {};

			}

			tail = transferOffsets[reclaimHint];

			reclaimHint++;
			reclaimHint %= fenceCount;
		}

		head = endOff;

		return { static_cast<uint8_t*>(pMapped) + baseOff , static_cast<uint8_t*>(pMapped) + endOff };
	}

	VkResult TransferStage::stageBufferUpload(const TransferSizeInfo* pSizeInfos, const TargetBufferInfo* pBufferInfos, uint32_t _TransferCount, VkSemaphore _Signal) {
		VkResult result;

		assert(pSizeInfos && pBufferInfos);

		size_t jobIndex;
		result = acquireTransferJob(&jobIndex);

		if (result != VK_SUCCESS)
			return result;

		VkCommandBuffer CMD = commandBuffers[jobIndex];

		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = nullptr;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			beginInfo.pInheritanceInfo = nullptr;

			result = vkBeginCommandBuffer(CMD, &beginInfo);
		}

		if (result != VK_SUCCESS)
			return result;

		VkDeviceSize flushBase = UINT64_MAX;
		VkDeviceSize flushEnd = 0;

		for (size_t i{}; i < _TransferCount; ++i) {
			mem::span alloc = allocate(pSizeInfos[i].size, pSizeInfos[i].alignment);

			if (!alloc) {

				continue;
			}

			memcpy(alloc.data(), pSizeInfos[i].pData, pSizeInfos[i].size);

			const VkDeviceSize allocBase = static_cast<VkDeviceSize>(static_cast<uint8_t*>(alloc.begin()) - static_cast<uint8_t*>(pMapped));
			flushBase = flushBase < allocBase ? flushBase : allocBase;

			const VkDeviceSize allocEnd = static_cast<VkDeviceSize>(static_cast<uint8_t*>(alloc.end()) - static_cast<uint8_t*>(pMapped));
			flushEnd = flushEnd < allocEnd ? allocEnd : flushEnd;

			VkBufferCopy copy = { allocBase, pBufferInfos[i].dstOffset, pSizeInfos[i].size };

			vkCmdCopyBuffer(CMD, stage, pBufferInfos[i].buffer, 1, &copy);
		}

		result = vkEndCommandBuffer(CMD);

		if (result != VK_SUCCESS)
			return result;

		const VkDeviceSize flushSize = flushEnd - flushBase;

		result = vmaFlushAllocation(r_allocator, allocation, flushBase, flushSize);

		if (result != VK_SUCCESS)
			return result;

		{
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pNext = nullptr;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &CMD;
			submitInfo.signalSemaphoreCount = _Signal ? 1 : 0;
			submitInfo.pSignalSemaphores = _Signal ? &_Signal : nullptr;

			result = vkQueueSubmit(transferQueue, 1, &submitInfo, transferFences[jobIndex]);
		}

		if (result != VK_SUCCESS)
			return result;

		transferOffsets[jobIndex] = flushEnd;

		stats.current = head >= tail ? static_cast<size_t>(head - tail) : static_cast<size_t>(bufferSize - tail + head);
		stats.peak = stats.current > stats.peak ? stats.current : stats.peak;

		return VK_SUCCESS;
	}

	VkResult TransferStage::submitUploads(VkCommandBuffer commandBuffer, VkFence fence) noexcept {
		const size_t bufferCount = stagingBufferStates.size();

		for (size_t i = 0; i < bufferCount; ++i) {
			stagingBufferState& bufferState = stagingBufferStates[i];

			if (bufferState.regions.empty())
				continue;

			std::unordered_map<VkBuffer, std::vector<VkBufferCopy>> batches;

			for (const auto& r : bufferState.regions) {
				batches[r.dstBuffer].push_back(r.copyInfo);
			}

			VkBuffer stagingBuffer = stagingBuffers[i].buffer;

			size_t flushSize = bufferState.flushEnd - bufferState.flushStart;
			vmaFlushAllocation(r_allocator, stagingBuffers.allocation(i), bufferState.flushStart, flushSize);

			for (auto& [dstBuffer, copyRegionInfo] : batches) {
				uint32_t copyRegionCount = static_cast<uint32_t>(copyRegionInfo.size());
				vkCmdCopyBuffer(commandBuffer, stagingBuffer, dstBuffer, copyRegionCount, copyRegionInfo.data());
			}

			bufferState.fence = fence;
		}

		return VK_SUCCESS;
	}

	void TransferStage::reset() noexcept {
		const size_t stageCount = stages.size();
		
		for (size_t i = 0; i < stageCount; ++i) {
			if (stages[i].fence)
				vkWaitForFences(r_device, 1, &stages[i].fence, VK_TRUE, UINT64_MAX);

			vmaDestroyBuffer(r_allocator, stages[i].buffer, stages[i].allocation);
				
		}

		stages.clear();
	}


}
