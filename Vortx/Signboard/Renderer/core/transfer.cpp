#include "transfer.h"

#include <array>
#include <unordered_map>

namespace rndr {

	static void comp_classWeightList(float* pList, uint32_t count, float spread) noexcept {
		spread = std::max(spread, 0.01f);

		const float invSigma2 = 1.0f / 2.0f * spread * spread;
		const float center = (float(count) - 1.0f) * 0.5f;

		float sum = 0.0f;

		for (uint32_t i = 0; i < count; ++i) {
			float deviation = float(i) - center;

			float weight = std::exp(-(deviation * deviation) * invSigma2);

			pList[i] = weight;
			sum += weight;
		}

		float invSum = 1.0f / sum;

		for (uint32_t i = 0; i < count; ++i) {
			pList[i] *= invSum;
		}
	}

	struct BufferAllocationLayoutCreateInfo {
		uint32_t sizeClassCount;
		VkDeviceSize memoryBudget;
		VkDeviceSize meanSize;
		float spread;
	};

	static void createBufferAllocationLayout(uint32_t* pAllocationsCounts, uint32_t* pTotalBufferCount, uint32_t* pBaseClassOffset, const BufferAllocationLayoutCreateInfo* pLayoutInfo) noexcept {
		uint32_t totalBufferCount = 0;

		uint32_t baseClassOffset;

		{
			unsigned long index;
			_BitScanReverse64(&index, pLayoutInfo->meanSize - 1);

			int preferredClass = int(index + 1);
			baseClassOffset = std::max(0, preferredClass - int(pLayoutInfo->sizeClassCount >> 1));

			std::vector<float> weights(pLayoutInfo->sizeClassCount);
			comp_classWeightList(weights.data(), pLayoutInfo->sizeClassCount, pLayoutInfo->spread);

			VkDeviceSize allocatedBytes = 0;

			for (size_t i = 0; i < pLayoutInfo->sizeClassCount; ++i) {
				VkDeviceSize classBudget = weights[i] * pLayoutInfo->memoryBudget;
				VkDeviceSize classSize = VkDeviceSize(1) << (baseClassOffset + i);

				float idealCount = float(classBudget) / float(classSize);
				uint32_t count = floor(idealCount);

				pAllocationsCounts[i] = count;
				totalBufferCount += count;

				weights[i] =  idealCount - float(count);

				allocatedBytes += VkDeviceSize(count) * classSize;
			}

			VkDeviceSize remainingBytes = pLayoutInfo->memoryBudget - allocatedBytes;

			while (remainingBytes) {
				unsigned long index;
				_BitScanReverse64(&index, remainingBytes);

				if (index < baseClassOffset)
					break;

				uint32_t fittingClassIndex = index - baseClassOffset;

				uint32_t classIndex = UINT32_MAX;
				float max = 0.0f;

				for (uint32_t i = 0; i <= fittingClassIndex; ++i) {
					if (weights[i] <= max)
						continue;

					classIndex = i;
					max = weights[i];
				}

				if (classIndex == UINT32_MAX)
					break;

				pAllocationsCounts[classIndex]++;
				totalBufferCount++;

				weights[classIndex] = -1.0f;

				remainingBytes -= VkDeviceSize(1) << (baseClassOffset + classIndex);
			}
		}

		*pBaseClassOffset = baseClassOffset;
		*pTotalBufferCount = totalBufferCount;
	}

	VkResult TransferStage::root(VkDevice device, VmaAllocator allocator, const TransferStageCreateInfo* pCreateInfo) noexcept {
		VkResult result{};

		VkCommandPool _commandPool = VK_NULL_HANDLE;

		std::vector<TransferJob> _jobs;

		std::vector<Stage> _stages;
		std::vector<VkDeviceSize> _stageSizes;

		uint32_t _baseClassOffset;
		std::vector<uint32_t> _sizeClassRange;

		do {
			{
				VkCommandPoolCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
				createInfo.queueFamilyIndex = pCreateInfo->transferFamilyIndex;

				result = vkCreateCommandPool(device, &createInfo, nullptr, &_commandPool);
			}

			if (result != VK_SUCCESS)
				break;

			_jobs.resize(pCreateInfo->maxConcurrentTransferJobs, TransferJob{});

			{
				VkCommandBufferAllocateInfo allocateInfo{};
				allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocateInfo.pNext = nullptr;
				allocateInfo.commandPool = _commandPool;
				allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocateInfo.commandBufferCount = 1;

				for (uint32_t i = 0; i < pCreateInfo->maxConcurrentTransferJobs; ++i) {
					result = vkAllocateCommandBuffers(device, &allocateInfo, &_jobs[i].commandBuffer);
					if (result != VK_SUCCESS)
						break;
				}
			}

			if (result != VK_SUCCESS)
				break;

			{
				VkFenceCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

				for (uint32_t i = 0; i < pCreateInfo->maxConcurrentTransferJobs; ++i) {
					result = vkCreateFence(device, &createInfo, nullptr, &_jobs[i].fence);
					if (result != VK_SUCCESS)
						break;
				}
			}

			if (result != VK_SUCCESS)
				break;

			uint32_t totalBufferAllcoations;

			std::vector<uint32_t> stageAllocationsCount(pCreateInfo->sizeClassCount);

			{
				BufferAllocationLayoutCreateInfo layoutInfo{};
				layoutInfo.sizeClassCount = pCreateInfo->sizeClassCount;
				layoutInfo.memoryBudget = pCreateInfo->stagingMemoryBudget;
				layoutInfo.meanSize = pCreateInfo->preferredTransferSize;
				layoutInfo.spread = pCreateInfo->transferSizeDistribution;

				createBufferAllocationLayout(stageAllocationsCount.data(), &totalBufferAllcoations, &_baseClassOffset, &layoutInfo);
			}

			_stages.resize(totalBufferAllcoations, Stage{});

			_stageSizes.resize(totalBufferAllcoations);
			
			_sizeClassRange.resize(pCreateInfo->sizeClassCount);

			{
				VkBufferCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				createInfo.size = 0;
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

				uint32_t classOffset = 0;

				for (uint32_t i = 0; i < pCreateInfo->sizeClassCount; ++i) {
					createInfo.size = VkDeviceSize(1) << (_baseClassOffset + i);

					for (uint32_t j = 0; j < stageAllocationsCount[i]; ++j) {
						Stage& stage = _stages[size_t(classOffset) + j];

						result = vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, &stage.buffer, &stage.allocation, &allocationInfo);
						if (result != VK_SUCCESS)
							break;

						_stages[size_t(classOffset) + j].pMapped = allocationInfo.pMappedData;
						_stageSizes[size_t(classOffset) + j] = allocationInfo.size;

					}
					if (result != VK_SUCCESS)
						break;

					classOffset += stageAllocationsCount[i];
					_sizeClassRange[i] = classOffset;
				}

			}

		} while (false);

		if(result == VK_SUCCESS) {
			const size_t jobCount = jobs.size();

			for (size_t i = 0; i < jobCount; ++i) {
				if (jobs[i].fence)
					vkWaitForFences(r_device, 1, &jobs[i].fence, VK_TRUE, UINT64_MAX);

				vkDestroyFence(r_device, jobs[i].fence, nullptr);
			}

			if (commandPool)
				vkDestroyCommandPool(r_device, commandPool, nullptr);
			
			const size_t stageCount = stages.size();

			for (size_t i = 0; i < stageCount; ++i) {
				vmaDestroyBuffer(r_allocator, stages[i].buffer, stages[i].allocation);
			}

			stages = std::move(_stages);
			availableStageSizes = std::move(_stageSizes);
			sizeClassRanges = std::move(_sizeClassRange);
			baseClassOffset = _baseClassOffset;

			commandPool = _commandPool;
			jobs = std::move(_jobs);

			transferFamilyIndex = pCreateInfo->transferFamilyIndex;
			transferQueue = pCreateInfo->transferQueue;

			r_device = device;
			r_allocator = allocator;

			return VK_SUCCESS;
		}

		const size_t bufferCount = _stages.size();

		for (uint32_t i = 0; i < bufferCount; ++i) {
			if (_stages[i].allocation)
				vmaDestroyBuffer(allocator, _stages[i].buffer, _stages[i].allocation);
		}

		const size_t jobCount = _jobs.size();

		for (uint32_t i = 0; i < jobCount; ++i) {
			if (_jobs[i].fence)
				vkDestroyFence(device, _jobs[i].fence, nullptr);
		}

		if (_commandPool)
			vkDestroyCommandPool(device, _commandPool, nullptr);


		return result;
	}

	constexpr static size_t alignUp(size_t _offset, size_t _alignment) noexcept {
		return (_offset + _alignment - 1) & ~(_alignment - 1);
	}

	VkResult TransferStage::stageBufferUpload(const BufferTransferInfo* pTransferInfos, uint32_t infoCount, TransferMode mode) {
		if (!pTransferInfos)
			return VK_ERROR_INVALID_EXTERNAL_HANDLE;

		const size_t bufferCount = stages.size();

		size_t bufferIndex = SIZE_MAX;


		for (size_t i = 0; i < bufferCount; ++i) {
			size_t index = (freeBufferHint + i) % bufferCount;

			Stage& stage = stages[index];

			if (stage.fence) {
				VkResult result = vkGetFenceStatus(r_device, stage.fence);

				if (result != VK_SUCCESS)
					continue;

				stage.currentOffset = 0;
				stage.flushStart = SIZE_MAX;
				stage.flushEnd = 0;

				stage.fence = VK_NULL_HANDLE;

				fre
			}

			size_t base = (stage.currentOffset + alignment - 1) & ~(alignment - 1);

			if (base + size > stage.size) {
				bufferRegions.push_back({ stage.buffer, target, { base, offset, static_cast<VkDeviceSize>(size) } });
				
				return VK_SUCCESS;
			}

		}

		size_t remaining = size;

		uint32_t i = 0;

		while (remaining) {
			size_t index = (freeBufferHint + i++) % bufferCount;


		}

		if (bufferIndex == SIZE_MAX)
			return VK_INCOMPLETE;

		Stage& stage = stages[bufferIndex];

		void* pDst = static_cast<char*>(stage.pMapped) + base;

		memcpy(pDst, pData, size);

		stage.currentOffset = offset + size;
		stage.flushStart = std::min(stage.flushStart, base);
		stage.flushEnd = std::max(stage.flushEnd, base + size);

		

		freeBufferHint = bufferIndex;

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
