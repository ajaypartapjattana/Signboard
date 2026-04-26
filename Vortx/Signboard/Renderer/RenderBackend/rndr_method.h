#pragma once

#include "Signboard/RHI/rhi.h"

#include "Signboard/Renderer/Pass/passes.h"
#include "Signboard/Renderer/Vertex/vertexFields.h"
#include "Signboard/Renderer/Materials/materials.h"

class RHIContext;
class rndr_presentation;
struct ResourceView;

struct rndr_method_Access;

struct methodView {
	ctnr::vltView<rhi::pmvRenderPass> renderPassView;
	ctnr::vltView<rhi::pmvFramebuffer> framebufferView;
	ctnr::vltView<rhi::pmvPipeline> pipelineView;
};

struct renderTarget {
	uint32_t renderPassIndex;
	std::vector<uint32_t> framebufferIndices;
	std::vector<uint32_t> pipelineIndices;
};

class rndr_method {
public:
	rndr_method(const RHIContext& context, const rndr_presentation& presentation, const ResourceView& view);

	rndr_method(const rndr_method&) = delete;
	rndr_method& operator=(const rndr_method&) = delete;

	struct renderTargetCreateInfo {
		passes::renderPassCreateInfo renderPassInfo;
		passes::framebufferCreateInfo framebufferInfo;
		vertexFields::createInfo VertexLayoutInfo;
		materials::pipelineCreateInfo pipelineInfo;
	};

	void create_renderTarget(renderTargetCreateInfo& creatInfo);

	const ctnr::vltView<renderTarget> read_renderTargets() const noexcept;

	void validate_primaryTarget(ctnr::span<const uint32_t> swapchainImageIndices);

private:
	VkResult create_primaryTarget(ctnr::span<const uint32_t> swapchainImageIndices);

private:
	friend struct rndr_method_Access;

	const rhi::creDevice& r_device;
	const rhi::pmvSwapchain& r_swapchain;
	const ResourceView& r_resources;

	passes m_passes;
	vertexFields m_fields;
	materials m_materials;

	uint32_t primaryTarget_idx;

	ctnr::vault<renderTarget> m_renderTargets;

};