#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Core/Containers/storage.h"

class passes;

class materials {
public:
	materials(const rhi::creDevice& device, const rhi::pmvSwapchain& swapchain, ctnr::vltView<rhi::pmvRenderPass> passAccess, ctnr::vltView<rhi::pmvVertexLayout> fieldsAccess);

	struct shaderBinary {
		VkShaderStageFlagBits stage;
		std::vector<uint32_t> data;
	};

	struct pipelineCreateInfo {
		uint32_t vertexLayoutIndex;
		std::vector<shaderBinary> shaders;
	};
	uint32_t createPipelineLayout(const std::vector<shaderBinary>& shaders);
	uint32_t createPipeline(uint32_t renderPassIndex, uint32_t subpass, uint32_t pipelineLayoutIndex, const pipelineCreateInfo& createInfo);

	ctnr::vltView<rhi::pmvPipeline> read_pipelines() const noexcept;

private:
	const rhi::creDevice& r_device;
	const rhi::pmvSwapchain& r_swapchain;

	const ctnr::vltView<rhi::pmvRenderPass> a_renderPassView;
	const ctnr::vltView<rhi::pmvVertexLayout> a_vertexLayoutView;

	ctnr::vault<rhi::pmvDescriptorLayout> m_descriptorLayouts;

	ctnr::vault<rhi::pmvPipelineLayout> m_pipelineLayouts;
	ctnr::vault<rhi::pmvPipeline> m_pipelines;

};