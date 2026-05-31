#pragma once

#include "Signboard/Core/Containers/span/span.h"
#include "Signboard/RHI/rhi.h"

struct shaderBinary {
	VkShaderStageFlagBits stage;
	sgb::span<const uint32_t> data;
};

struct renderPipelineShaderSet {
	shaderBinary vertexShaderBin;
	shaderBinary fragmentShaderBin;

	std::array<shaderBinary, 2> list() const noexcept {
		return {
			vertexShaderBin,
			fragmentShaderBin
		};
	}
};

struct renderPassContext {
	VkRenderPass renderPass;
	std::vector<VkFramebuffer> framebuffers;
	std::vector<VkExtent2D> extent;
};

namespace rndr {

	class renderConfig {
	private:
		resource_pool<rhi::renderPass> renderPasses;

		resource_pool<rhi::shaderModule> shaderModules;
		resource_pool<rhi::descriptorSetLayout> descriptorSetLayouts;
		resource_pool<rhi::pipelineLayout> pipelineLayouts;
		resource_pool<rhi::graphicsPipeline> grpahicsPipelines;

		resource_pool<rhi::framebuffer> framebuffers;

		std::vector<renderPassContext> renderContexts;

	public:
		renderConfig() = default;
		renderConfig(VkDevice device) noexcept
			:
			renderPasses(device),
			shaderModules(device),
			descriptorSetLayouts(device),
			pipelineLayouts(device),
			grpahicsPipelines(device),
			framebuffers(device)
		{

		}
		renderConfig(const renderConfig&) = delete;
		renderConfig(renderConfig&& other) noexcept 
			:
			renderPasses(std::move(other.renderPasses)),
			shaderModules(std::move(other.shaderModules)),
			descriptorSetLayouts(std::move(other.descriptorSetLayouts)),
			pipelineLayouts(std::move(other.pipelineLayouts)),
			grpahicsPipelines(std::move(other.grpahicsPipelines)),
			framebuffers(std::move(other.framebuffers)),
			renderContexts(std::move(other.renderContexts))
		{

		}

		renderConfig& operator=(const renderConfig&) = delete;
		renderConfig& operator=(renderConfig&& other) noexcept {
			if (this == &other)
				return *this;
			
			renderPasses = std::move(other.renderPasses);
			shaderModules = std::move(other.shaderModules);
			descriptorSetLayouts = std::move(other.descriptorSetLayouts);
			pipelineLayouts = std::move(other.pipelineLayouts);
			grpahicsPipelines = std::move(other.grpahicsPipelines);
			framebuffers = std::move(other.framebuffers);
			renderContexts = std::move(other.renderContexts);

			return *this;
		}

		~renderConfig() noexcept {

		}

		uint32_t createBackbufferRenderContext(VkFormat swapchainImageFormat);
		void createFramebuffer(uint32_t renderContextIndex, sgb::span<const VkImageView> attachments, VkExtent2D extent);

		void createRenderPipeline(uint32_t compatibilityContextIndex, renderPipelineShaderSet shaders);

		void validate_primaryTarget();

	};

}
