#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

namespace rhi {

	class creDevice;

	class pmvSwapchain;

	class pmvVertexLayout;
	class pmvPipelineLayout;
	class pmvRenderPass;
	class pmvPipeline;
	class pmvShader;

	class pcdPipelineBuilder {
	public:
		pcdPipelineBuilder(const rhi::creDevice& device, const rhi::pmvSwapchain& swapchain, const rhi::pmvPipelineLayout& pipelineLayout) noexcept;

		pcdPipelineBuilder& set_vertexLayout(const pmvVertexLayout* layout) noexcept;

		pcdPipelineBuilder& set_vertShader(const rhi::pmvShader& shader) noexcept;
		pcdPipelineBuilder& set_fragShader(const rhi::pmvShader& shader) noexcept;

		pcdPipelineBuilder& set_targetPass(const rhi::pmvRenderPass* renderPass) noexcept;

		VkResult build_graphicsPipeline(const uint32_t target_subpass, rhi::pmvPipeline& pipeline);

	private:
		VkDevice m_device;
		
		VkPipelineLayout m_pipelineLayout;

		const pmvRenderPass* r_renderPass;
		const pmvVertexLayout* r_vertexLayout;

		VkExtent2D m_viewportExtent{};

		std::vector<VkShaderModule> m_modules;
		std::vector<VkShaderStageFlagBits> m_moduleRef;

	};

}