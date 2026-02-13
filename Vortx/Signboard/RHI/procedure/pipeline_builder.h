#pragma once

namespace rhi::core {
	class device;
	class swapchain;
}

namespace rhi::primitive {
	class pipelineLayout;
	class renderPass;
	class pipeline;
	class shader;
}

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::procedure {

	class pipeline_builder {
	public:
		pipeline_builder(const rhi::core::device& device, const rhi::core::swapchain& swapchain, const rhi::primitive::pipelineLayout& pipelineLayout);

		pipeline_builder& set_vertShader(const rhi::primitive::shader& shader) noexcept;
		pipeline_builder& set_fragShader(const rhi::primitive::shader& shader) noexcept;

		VkResult build_graphicsPipeline(const rhi::primitive::renderPass& renderPass, rhi::primitive::pipeline& pipeline);

	private:
		VkDevice m_device;

		VkPipelineLayout m_layout;

		std::vector<VkShaderModule> m_modules;
		std::vector<VkShaderStageFlagBits> m_moduleRef;

		VkExtent2D m_viewportExtent;

	};

}