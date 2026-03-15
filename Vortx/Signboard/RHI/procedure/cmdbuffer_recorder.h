#pragma once

namespace rhi::primitive {
	class commandBuffer;

	class renderPass;
	class framebuffer;
	class pipeline;
}

#include <vulkan/vulkan.h>

namespace rhi::procedure {

	class cmdbuffer_recorder {
	public:
		cmdbuffer_recorder(const rhi::primitive::commandBuffer& buffer);

		void begin_renderTarget(const rhi::primitive::renderPass& renderPass, const rhi::primitive::framebuffer& target) noexcept;
		void bind_pipeline(const rhi::primitive::pipeline& pipeline, const bool dynamicStateEnabled) const noexcept;

		void draw();

		void end_renderTarget() const;
		VkResult end_recording() const;

	private:
		void reset_buffer();
		VkResult begin_recording();

	private:
		VkCommandBuffer m_buffer;

		VkExtent2D a_targetExtent;

	};

}