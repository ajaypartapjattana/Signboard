#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

#include "Signboard/Core/core.h"

namespace rhi {

	class device;

	class pmvRenderPass;
	class pmvPipelineLayout;
	class pmvVertexLayout;
	class pmvShader;

	class pmvPipeline;

	struct graphicPipelineCreateArena {
		VkPipelineVertexInputStateCreateInfo vertexInputState;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
		VkPipelineTessellationStateCreateInfo tessellationState;
		VkPipelineViewportStateCreateInfo viewportState;
		VkPipelineRasterizationStateCreateInfo rasterizationState;
		VkPipelineMultisampleStateCreateInfo multisampleState;
		VkPipelineDepthStencilStateCreateInfo depthStencilState;
		VkPipelineColorBlendStateCreateInfo colorBlendState;
		VkPipelineDynamicStateCreateInfo dynamicState;
	};

	class pcdGraphicPipelineCreate {
	public:
		pcdGraphicPipelineCreate(const device& device, graphicPipelineCreateArena* pCreateArena = nullptr, VkGraphicsPipelineCreateInfo* pCreateInfo = nullptr) noexcept;

		void bind_shaders(sgb::span<const pmvShader> shaders) noexcept;

		void vertexInput(sgb::span<const VkVertexInputBindingDescription> bindings, sgb::span<const VkVertexInputAttributeDescription> attributes) noexcept;
		void inputAssembly(VkPrimitiveTopology topology) noexcept;
		void viewportState(uint32_t viewportCount, uint32_t scissorsCount) noexcept;

		void rasterCulling(VkBool32 discard, VkCullModeFlags cullMode, VkFrontFace frontFace) noexcept;
		void rasterPolygons(VkPolygonMode mode, float lineWidth) noexcept;
		void rasterDepthClamping(VkBool32 depthClamp) noexcept;
		void rasterDepthBias(VkBool32 depthBias, float factor, float clamp, float slope) noexcept;

		void multisample(VkSampleCountFlagBits sampleCount) noexcept;
		void depthStencilState() noexcept;
		void push_blendAttachments(sgb::span<const VkPipelineColorBlendAttachmentState> blendAttachments) noexcept;
		void dynamicStates(sgb::span<const VkDynamicState> states) noexcept;

		void target_renderPass(const pmvRenderPass& renderPass, uint32_t subpass) noexcept;
		void target_layout(const pmvPipelineLayout& pipelineLayout) noexcept;

		VkResult publish(pmvPipeline& pipeline);

		void preset(graphicPipelineCreateArena* pCreateArena, VkGraphicsPipelineCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		graphicPipelineCreateArena* allot_arena(graphicPipelineCreateArena* pArena) noexcept;
		VkGraphicsPipelineCreateInfo* allot_basic(VkGraphicsPipelineCreateInfo* pCreateInfo, graphicPipelineCreateArena& arena) noexcept;

	private:
		const VkDevice r_device;

		std::vector<VkPipelineShaderStageCreateInfo> m_shaders;

		std::unique_ptr<graphicPipelineCreateArena> m_ownedArena;
		graphicPipelineCreateArena* pArena;

		std::unique_ptr<VkGraphicsPipelineCreateInfo> m_ownedInfo;
		VkGraphicsPipelineCreateInfo* pInfo;

	};

}