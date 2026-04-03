#pragma once

#include "Signboard/RHI/rhi.h"

#include "Signboard/Renderer/Pass/passes.h"
#include "Signboard/Renderer/Vertex/vertexFields.h"
#include "Signboard/Renderer/Materials/materials.h"

class RHIContext;
class rndr_presentation;

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
	rndr_method(const RHIContext& context, const rndr_presentation& presentation);

	rndr_method(const rndr_method&) = delete;
	rndr_method& operator=(const rndr_method&) = delete;

	struct createInfo {
		passes::createInfo passInfo;
		vertexFields::createInfo fieldInfo;
		materials::createInfo materialInfo;
	};
	void create_renderTarget(const createInfo& info);
	const ctnr::vltView<renderTarget> read_renderTargets() const noexcept;

	void validate_primaryTarget();

private:
	void create_primaryTarget();

private:
	friend struct rndr_method_Access;

	const rhi::creDevice& r_device;
	const rhi::pmvSwapchain& r_swapchain;

	passes m_passes;
	vertexFields m_fields;
	materials m_materials;

	uint32_t m_primaryTarget_handle;
	ctnr::vault<renderTarget> targets;
	ctnr::vault_writeAccessor<renderTarget> m_writeAccess;

};