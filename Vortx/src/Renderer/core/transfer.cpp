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
		mem::span<uint8_t*> _transferOffsets;

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

			_transferOffsets = _stack.alloc<uint8_t*>(pCreateInfo->maxConcurrentTransferJobs);
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
			transferMarks = _transferOffsets;

			jobBuffers.reserve(8);

			stage = _stage;
			allocation = _allocation;
			pMapped = static_cast<uint8_t*>(_pMapped);
			pEnd = pMapped + _allocSize;

			pHead = static_cast<uint8_t*>(_pMapped);
			pTail = static_cast<uint8_t*>(_pMapped);

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

	VkResult TransferStage::waitTransferJob(size_t _Index) noexcept {
		VkResult result;

		result = vkGetFenceStatus(r_device, transferFences[_Index]);
		switch (result) {
		case VK_SUCCESS:

			break;

		case VK_NOT_READY:
			stats.stalls++;

			result = vkWaitForFences(r_device, 1, &transferFences[_Index], VK_TRUE, UINT64_MAX);

			if (result != VK_SUCCESS)
				return result;

			break;

		default:
			return result;

		}

		return VK_SUCCESS;
	}

	VkResult TransferStage::createUniqueBuffer(size_t _Size, UniqueBuffer* pUniqueBuffer, uint8_t** pBufferData) const noexcept {
		VkResult result;
		
		assert(pUniqueBuffer && pBufferData);

		VkBuffer _buffer;
		VmaAllocation _allocation;
		VmaAllocationInfo _info;

		{
			VkBufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.size = _Size;
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

			result = vmaCreateBuffer(r_allocator, &createInfo, &allocationCreateInfo, &_buffer, &_allocation, &_info);
		}
		
		if (result != VK_SUCCESS) {
			*pBufferData = nullptr;

			return result;
		}

		*pBufferData = static_cast<uint8_t*>(_info.pMappedData);

		pUniqueBuffer->buffer = _buffer;
		pUniqueBuffer->allocation = _allocation;

		return VK_SUCCESS;
	}


	VkResult TransferStage::stageBufferUpload(const TransferSizeInfo* pSource, const TargetBufferInfo* pBufferInfos, uint32_t _TransferCount, VkSemaphore _Signal) noexcept {
		VkResult result;

		assert(pSource && pBufferInfos);

		size_t transferIndex = 0;

		while (transferFamilyIndex < _TransferCount) {
			result = waitTransferJob(acquireHint);

			if (result != VK_SUCCESS)
				return result;

			size_t jobIndex = acquireHint;

			const size_t jobCount = transferFences.size();

			acquireHint++;
			acquireHint %= jobCount;

			pTail = transferMarks[jobIndex];

			result = vkResetFences(r_device, 1, &transferFences[jobIndex]);

			if (result != VK_SUCCESS)
				return result;

			result = vkResetCommandBuffer(commandBuffers[jobIndex], 0);

			if (result != VK_SUCCESS)
				return result;

			{
				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.pNext = nullptr;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
				beginInfo.pInheritanceInfo = nullptr;

				result = vkBeginCommandBuffer(commandBuffers[jobIndex], &beginInfo);
			}

			if (result != VK_SUCCESS)
				return result;

			VkDeviceSize flushBase = UINT64_MAX;
			VkDeviceSize flushEnd = 0;

			while (transferIndex < _TransferCount) {
				uint8_t* pAllocBase = mem::alignUp<uint8_t>(pHead, pSource[transferIndex].alignment);
				uint8_t* pAllocEnd = pAllocBase + pSource[transferIndex].size;

				if (pAllocEnd > pEnd) {
					
					pAllocBase = mem::alignUp<uint8_t>(pMapped, pSource[transferIndex].alignment);
					pAllocEnd = pAllocBase + pSource[transferIndex].size;

					if (pAllocEnd < pEnd) {
						pHead = pMapped;

						break;
					}

					UniqueBuffer& jobBuffer = jobBuffers.emplace_back();
					uint8_t* jobBufferData;

					result = createUniqueBuffer(pSource[transferIndex].size, &jobBuffer, &jobBufferData);

					if (result != VK_SUCCESS)
						return result;

					memcpy(jobBufferData, pSource[transferIndex].pData, pSource[transferIndex].size);

					result = vmaFlushAllocation(r_allocator, jobBuffer.allocation, 0, pSource[transferIndex].size);

					if (result != VK_SUCCESS) {
						return result;
					}

					VkBufferCopy copy = { 0, pBufferInfos[transferIndex].dstOffset, pSource[transferIndex].size };
					
					vkCmdCopyBuffer(commandBuffers[jobIndex], jobBuffer.buffer, pBufferInfos[transferIndex].buffer, 1, &copy);

					jobBufferFences.push_back(transferFences[jobIndex]);
				}

				if (pHead < pTail && pAllocEnd > pTail) {
					size_t i = acquireHint;

					do {
						result = waitTransferJob(i);

						if (result != VK_SUCCESS)
							return result;

						pTail = transferMarks[i % jobCount];

						if (pAllocEnd < pTail)
							break;

						i++;
						i %= jobCount;

					} while (i != jobIndex);

					if (pAllocEnd > pTail) {
						break;
					}
				}

				pHead = pAllocEnd;

				memcpy(pAllocBase, pSource[transferIndex].pData, pSource[transferIndex].size);

				const VkDeviceSize allocBase = static_cast<VkDeviceSize>(pAllocBase - pMapped);
				flushBase = flushBase < allocBase ? flushBase : allocBase;

				const VkDeviceSize allocEnd = static_cast<VkDeviceSize>(pAllocEnd - pMapped);
				flushEnd = flushEnd < allocEnd ? allocEnd : flushEnd;

				VkBufferCopy copy = { allocBase, pBufferInfos[transferIndex].dstOffset, pSource[transferIndex].size };

				vkCmdCopyBuffer(commandBuffers[jobIndex], stage, pBufferInfos[transferIndex].buffer, 1, &copy);

				transferIndex++;
			}

			result = vkEndCommandBuffer(commandBuffers[jobIndex]);

			if (result != VK_SUCCESS)
				return result;

			stats.current = pHead >= pTail ? static_cast<size_t>(pHead - pTail) : static_cast<size_t>(pEnd - pTail + pHead - pMapped);
			stats.peak = stats.current > stats.peak ? stats.current : stats.peak;

			if (!flushEnd)
				continue;

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
				submitInfo.pCommandBuffers = &commandBuffers[jobIndex];
				submitInfo.signalSemaphoreCount = _Signal ? 1 : 0;
				submitInfo.pSignalSemaphores = _Signal ? &_Signal : nullptr;

				result = vkQueueSubmit(transferQueue, 1, &submitInfo, transferFences[jobIndex]);
			}

			if (result != VK_SUCCESS)
				return result;

			transferMarks[jobIndex] = pMapped + flushEnd;
		}

		return VK_SUCCESS;
	}

	VkResult TransferStage::dumpUniqueBuffers() noexcept {
		VkResult result;

		if (jobBuffers.empty())
			return VK_SUCCESS;

		result = vkWaitForFences(r_device, static_cast<uint32_t>(jobBufferFences.size()), jobBufferFences.data(), VK_TRUE, UINT64_MAX);
		
		if (result != VK_SUCCESS)
			return result;

		UniqueBuffer* const pLast = jobBuffers.end()._Ptr;

		for (UniqueBuffer* pBuffer{ jobBuffers.data() }; pBuffer != pLast; ++pBuffer) {
			vmaDestroyBuffer(r_allocator, pBuffer->buffer, pBuffer->allocation);
		}

		jobBuffers.clear();
		jobBufferFences.clear();

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
