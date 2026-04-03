#include "commandBufferRecorder.h"

#include "Signboard/RHI/primitive/commandBuffer_pAccess.h"
#include "Signboard/RHI/primitive/framebuffer_pAccess.h"
#include "Signboard/RHI/primitive/renderPass_pAccess.h"
#include "Signboard/RHI/primitive/pipeline_pAccess.h"

#include "Signboard/RHI/primitive/buffer_pAccess.h"

namespace rhi {

	pcdCommandBufferRecorder::pcdCommandBufferRecorder(const rhi::pmvCommandBuffer& buffer)
		: 
		m_buffer(rhi::access::commandBuffer_pAccess::get(buffer))
	{
		reset_buffer();
		begin_recording();
	}

	void pcdCommandBufferRecorder::reset_buffer() const {
		vkResetCommandBuffer(m_buffer, 0);
	}

	VkResult pcdCommandBufferRecorder::begin_recording() const {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		return vkBeginCommandBuffer(m_buffer, &beginInfo);
	}

	void pcdCommandBufferRecorder::begin_renderTarget(const rhi::pmvRenderPass& renderPass, const rhi::pmvFramebuffer& framebuffer) noexcept {
		a_targetExtent = rhi::access::framebuffer_pAccess::get_extent(framebuffer);

		VkRenderPassBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.renderPass = rhi::access::renderPass_pAccess::get(renderPass);
		beginInfo.framebuffer = rhi::access::framebuffer_pAccess::get(framebuffer);

		beginInfo.renderArea.offset = { 0, 0 };
		beginInfo.renderArea.extent = a_targetExtent;

		VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
		beginInfo.clearValueCount = 1;
		beginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(m_buffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void pcdCommandBufferRecorder::bind_pipeline(const rhi::pmvPipeline& pipeline, const bool dynamicStateEnabled) const noexcept {
		VkPipeline a_pipeline = rhi::access::pipeline_pAccess::get(pipeline);
		VkPipelineBindPoint a_type = rhi::access::pipeline_pAccess::get_type(pipeline);
		
		vkCmdBindPipeline(m_buffer, a_type, a_pipeline);

		if (!dynamicStateEnabled)
			return;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(a_targetExtent.width);
		viewport.height = static_cast<float>(a_targetExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_buffer, 0, 1, &viewport);
		
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = a_targetExtent;
		vkCmdSetScissor(m_buffer, 0, 1, &scissor);

	}

	void pcdCommandBufferRecorder::bind_vertexBuffer(const rhi::pmvBuffer& buffer) const noexcept {
		VkBuffer a_vertBuffer = rhi::access::buffer_pAccess::get(buffer);

		VkBuffer toBindBuffer[] = { a_vertBuffer };

		vkCmdBindVertexBuffers(m_buffer, 0, 1, toBindBuffer, 0);
	}

	void pcdCommandBufferRecorder::bind_indexBuffer(const rhi::pmvBuffer& buffer) const noexcept {
		VkBuffer a_indexBuffer = rhi::access::buffer_pAccess::get(buffer);

		vkCmdBindIndexBuffer(m_buffer, a_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void pcdCommandBufferRecorder::draw() {
		vkCmdDraw(m_buffer, 3, 1, 0, 0);
	}

	void pcdCommandBufferRecorder::end_renderTarget() const {
		vkCmdEndRenderPass(m_buffer);
	}

	VkResult pcdCommandBufferRecorder::end_recording() const {
		return vkEndCommandBuffer(m_buffer);
	}

	void pcdCommandBufferRecorder::uploadBuffer(const rhi::pmvBuffer& src, const rhi::pmvBuffer& dst, const std::vector<VkBufferCopy>& copyInfo) const {
		VkBuffer _src = rhi::access::buffer_pAccess::get(src);
		VkBuffer _dst = rhi::access::buffer_pAccess::get(dst);
		
		uint32_t regionCount = static_cast<uint32_t>(copyInfo.size());

		vkCmdCopyBuffer(m_buffer, _src, _dst, regionCount, copyInfo.data());
	}


}