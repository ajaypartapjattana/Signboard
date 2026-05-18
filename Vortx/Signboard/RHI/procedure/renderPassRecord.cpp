#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdRenderPassRecord::pcdRenderPassRecord(VkRenderPassBeginInfo* pBeginInfo) noexcept
		:
		pInfo(allot_basic(pBeginInfo))
	{

	}

	VkRenderPassBeginInfo* pcdRenderPassRecord::allot_basic(VkRenderPassBeginInfo* pBeginInfo) noexcept {
		if (pBeginInfo)
			return pBeginInfo;

		m_ownedInfo = std::make_unique<VkRenderPassBeginInfo>();

		VkRenderPassBeginInfo* _info = m_ownedInfo.get();
		_info->sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

		return _info;
	}

	void pcdRenderPassRecord::target_commandBuffer(const pmvCommandBuffer& commandbuffer) noexcept {
		r_commandBuffer = _pAccess::extract(commandbuffer);
	}

	void pcdRenderPassRecord::target_clearValues(sgb::span<const VkClearValue> clearValues) {
		pInfo->pClearValues = clearValues.data();
		pInfo->clearValueCount = static_cast<uint32_t>(clearValues.size());
	}

	void pcdRenderPassRecord::renderArea(VkRect2D rect) noexcept {
		pInfo->renderArea = rect;
	}

	void pcdRenderPassRecord::begin(const pmvRenderPass& renderPass, const pmvFramebuffer& framebuffer) noexcept {
		pInfo->renderPass = _pAccess::extract(renderPass);
		pInfo->framebuffer = _pAccess::extract(framebuffer);

		vkCmdBeginRenderPass(r_commandBuffer, pInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void pcdRenderPassRecord::bind_pipeline(const pmvPipeline& pipeline) const noexcept {
		vkCmdBindPipeline(r_commandBuffer, _pAccess::type(pipeline), _pAccess::extract(pipeline));
	}

	void pcdRenderPassRecord::viewport(sgb::span<const VkViewport> viewports, sgb::span<const VkRect2D> scissors) const noexcept {
		vkCmdSetViewport(r_commandBuffer, 0, static_cast<uint32_t>(viewports.size()), viewports.data());
		vkCmdSetScissor(r_commandBuffer, 0, static_cast<uint32_t>(scissors.size()), scissors.data());
	}

	void pcdRenderPassRecord::bind_vertexBuffer(sgb::span<const pmvBuffer*> vertexBuffers, sgb::span<const VkDeviceSize> offsets) const noexcept {
		size_t _vbSz = vertexBuffers.size();

		std::vector<VkBuffer> buffers(_vbSz);
		for (size_t i = 0; i < _vbSz; ++i) 
			buffers[i] = _pAccess::extract(*vertexBuffers[i]);

		vkCmdBindVertexBuffers(r_commandBuffer, 0, static_cast<uint32_t>(_vbSz), buffers.data(), offsets.data());
	}

	void pcdRenderPassRecord::bind_indexBuffer(const pmvBuffer& buffer) const noexcept {
		vkCmdBindIndexBuffer(r_commandBuffer, _pAccess::extract(buffer), 0, VK_INDEX_TYPE_UINT32);
	}

	void pcdRenderPassRecord::draw(uint32_t indexCount) const noexcept {
		vkCmdDrawIndexed(r_commandBuffer, indexCount, 1, 0, 0, 0);
	}

	void pcdRenderPassRecord::end() const noexcept {
		vkCmdEndRenderPass(r_commandBuffer);
	}

}