#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdCommandBufferAllocate::pcdCommandBufferAllocate(const creDevice& device, VkCommandBufferAllocateInfo* pAllocInfo) noexcept
		: 
		r_device(_pAccess::extract(device)),
		pInfo(allot_basic(pAllocInfo))
	{
	
	}

	VkCommandBufferAllocateInfo* pcdCommandBufferAllocate:: allot_basic(VkCommandBufferAllocateInfo* pAllocInfo) noexcept {
		if (pAllocInfo)
			return pAllocInfo;

		m_ownedInfo = std::make_unique<VkCommandBufferAllocateInfo>();

		VkCommandBufferAllocateInfo* _info = m_ownedInfo.get();

		VkCommandBufferAllocateInfo& info = *_info;
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

		return _info;
	}

	VkResult pcdCommandBufferAllocate::target_commandPool(const pmvCommandPool& commandPool) noexcept {
		VkCommandPool _pool = _pAccess::extract(commandPool);

#ifdef _VALIDATE
		if (!_pool)
			return VK_ERROR_INVALID_EXTERNAL_HANDLE;
#endif

		pInfo->commandPool = _pool;

		return VK_SUCCESS;
	}

	void pcdCommandBufferAllocate::secondryBuffers(bool enable) noexcept {
		pInfo->level = enable ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	}

	VkResult pcdCommandBufferAllocate::publish(sgb::span<pmvCommandBuffer> targets) noexcept {
		if (targets.empty())
			return VK_INCOMPLETE;

		size_t _bSz = targets.size();

		std::vector<VkCommandBuffer> _buffers(_bSz);

		pInfo->commandBufferCount = static_cast<uint32_t>(_bSz);
		
		VkResult result = vkAllocateCommandBuffers(r_device, pInfo, _buffers.data());
		if (result != VK_SUCCESS)
			return result;

		free_buffers(targets);

		VkCommandPool _pool = pInfo->commandPool;
		for (uint32_t i = 0; i < _bSz; ++i) {
			targets[i].m_commandBuffer = _buffers[i];
			targets[i].r_commandPool = _pool;
		}

		return VK_SUCCESS;
	}

	void pcdCommandBufferAllocate::free_buffers(sgb::span<pmvCommandBuffer> buffers) const noexcept {
		std::vector<VkCommandBuffer> batch;

		size_t _bSz = buffers.size();
		VkCommandPool batch_pool = VK_NULL_HANDLE;
		for (uint32_t i = 0; i < _bSz; ++i) {
			if (buffers[i].r_commandPool == VK_NULL_HANDLE)
				continue;

			batch_pool = buffers[i].r_commandPool;
			break;
		}

		batch.reserve(_bSz);
		for (uint32_t i = 0; i < _bSz; ++i) {
			if (buffers[i].r_commandPool == VK_NULL_HANDLE)
				continue;

			if (buffers[i].r_commandPool != batch_pool) {
				if(!batch.empty())
					vkFreeCommandBuffers(r_device, batch_pool, static_cast<uint32_t>(batch.size()), batch.data());

				batch.clear();
				batch_pool = buffers[i].r_commandPool;
			}

			batch.push_back(buffers[i].m_commandBuffer);
		}

		if (!batch.empty())
			vkFreeCommandBuffers(r_device, batch_pool, static_cast<uint32_t>(batch.size()), batch.data());
	}


}