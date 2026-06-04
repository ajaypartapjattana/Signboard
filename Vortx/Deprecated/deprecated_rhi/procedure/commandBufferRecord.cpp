#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdCommandBufferRecord::pcdCommandBufferRecord(VkCommandBufferBeginInfo* pBegininfo) noexcept
		: 
		pBeginInfo(allot_basic(pBegininfo))
	{

	}

	VkCommandBufferBeginInfo* pcdCommandBufferRecord::allot_basic(VkCommandBufferBeginInfo* pBeginInfo) noexcept {
		if (pBeginInfo)
			return pBeginInfo;

		m_ownedBeginInfo = std::make_unique<VkCommandBufferBeginInfo>();

		VkCommandBufferBeginInfo* _info = m_ownedBeginInfo.get();
		_info->sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		return _info;
	}

	void pcdCommandBufferRecord:: target_buffer(const pmvCommandBuffer& commandBuffer) noexcept {
		r_commandBuffer = _pAccess::extract(commandBuffer);
	}

	void pcdCommandBufferRecord::reset_buffer(VkCommandBufferResetFlags flags) const noexcept {
		vkResetCommandBuffer(r_commandBuffer, flags);
	}

	VkResult pcdCommandBufferRecord::begin() const {
		return vkBeginCommandBuffer(r_commandBuffer, pBeginInfo);
	}

	void pcdCommandBufferRecord::uploadBuffer(const pmvBuffer& src, const pmvBuffer& dst, sgb::span<const VkBufferCopy> copyInfo) const {
		uint32_t regionCount = static_cast<uint32_t>(copyInfo.size());
		vkCmdCopyBuffer(r_commandBuffer, _pAccess::extract(src), _pAccess::extract(dst), regionCount, copyInfo.data());
	}

	VkResult pcdCommandBufferRecord::end() const noexcept {
		return vkEndCommandBuffer(r_commandBuffer);
	}

}

