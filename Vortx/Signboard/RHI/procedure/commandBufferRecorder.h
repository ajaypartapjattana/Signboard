#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	class pmvCommandBuffer;

	class pmvRenderPass;
	class pmvFramebuffer;
	class pmvPipeline;

	class pcdCommandBufferRecorder {
	public:
		pcdCommandBufferRecorder(const rhi::pmvCommandBuffer& buffer);

		void begin_renderTarget(const rhi::pmvRenderPass& renderPass, const rhi::pmvFramebuffer& target) noexcept;
		void bind_pipeline(const rhi::pmvPipeline& pipeline, const bool dynamicStateEnabled) const noexcept;

		void draw();

		void end_renderTarget() const;
		VkResult end_recording() const;

	private:
		void reset_buffer() const;
		VkResult begin_recording();

	private:
		VkCommandBuffer m_buffer;

		VkExtent2D a_targetExtent;

	};

}