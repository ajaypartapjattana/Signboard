#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	class pmvCommandBuffer;

	class pmvRenderPass;
	class pmvFramebuffer;
	class pmvPipeline;

	class pmvBuffer;

	class pcdCommandBufferRecorder {
	public:
		pcdCommandBufferRecorder(const rhi::pmvCommandBuffer& buffer);
		
		VkResult end_recording() const;

		void begin_renderTarget(const rhi::pmvRenderPass& renderPass, const rhi::pmvFramebuffer& target) noexcept;
		void bind_pipeline(const rhi::pmvPipeline& pipeline, const bool dynamicStateEnabled) const noexcept;
		void bind_vertexBuffer(const rhi::pmvBuffer& buffer) const noexcept;
		void bind_indexBuffer(const rhi::pmvBuffer& buffer) const noexcept;
		void draw(uint32_t indexCount) const;
		void end_renderTarget() const;

		void uploadBuffer(const rhi::pmvBuffer& src, const rhi::pmvBuffer& dst, const std::vector<VkBufferCopy>& copyInfo) const;

	private:
		void reset_buffer() const;
		VkResult begin_recording() const;

	private:
		VkCommandBuffer m_buffer;

		VkExtent2D a_targetExtent;

	};

}