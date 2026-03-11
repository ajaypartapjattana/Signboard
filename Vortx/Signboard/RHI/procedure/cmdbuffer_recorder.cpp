#include "cmdbuffer_recorder.h"

#include "Signboard/RHI/primitive/commandBuffer_vk.h"
#include "Signboard/RHI/primitive/framebuffer_vkAccess.h"
#include "Signboard/RHI/primitive/pipeline_vk.h"

namespace rhi::procedure {

	cmdbuffer_recorder::cmdbuffer_recorder(const rhi::primitive::commandBuffer& buffer)
		: m_buffer(rhi::primitive::commandBuffer_vkAccess::get(buffer))
	{
		begin_recording();
	}

	VkResult cmdbuffer_recorder::begin_recording() {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		return vkBeginCommandBuffer(m_buffer, &beginInfo);
	}

	void cmdbuffer_recorder::begin_renderTarget(const rhi::primitive::framebuffer& target) noexcept {
		a_targetExtent = rhi::primitive::framebuffer_vkAccess::get_extent(target);

		VkRenderPassBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.renderPass = rhi::primitive::framebuffer_vkAccess::get_nativePass(target);
		beginInfo.framebuffer = rhi::primitive::framebuffer_vkAccess::get(target);

		beginInfo.renderArea.offset = { 0, 0 };
		beginInfo.renderArea.extent = a_targetExtent;

		VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
		beginInfo.clearValueCount = 1;
		beginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(m_buffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void cmdbuffer_recorder::bind_pipeline(const rhi::primitive::pipeline& pipeline, const bool dynamicStateEnabled) const noexcept {
		VkPipeline a_pipeline = rhi::primitive::pipeline_vkAccess::get(pipeline);
		VkPipelineBindPoint a_type = rhi::primitive::pipeline_vkAccess::get_type(pipeline);
		
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

	void cmdbuffer_recorder::draw() {
		vkCmdDraw(m_buffer, 3, 1, 0, 0);
	}

	void cmdbuffer_recorder::end_renderTarget() const {
		vkCmdEndRenderPass(m_buffer);
	}

	VkResult cmdbuffer_recorder::end_recording() const {
		return vkEndCommandBuffer(m_buffer);
	}

}