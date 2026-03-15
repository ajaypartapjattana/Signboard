#pragma once

class rndr_context;
class rndr_presentation;

#include "Signboard/RHI/rhi.h"

#include "Signboard/Renderer/Pass/passes.h"
#include "Signboard/Renderer/Materials/materials.h"

struct rndr_method_Access;

struct render_target {
	storage::storage_handle pass;
	std::vector<storage::storage_handle> framebuffers;
	std::vector<storage::storage_handle> pipelines;
};

class rndr_method {
public:
	rndr_method(const rndr_context& context, const rndr_presentation& presentation);

	rndr_method(const rndr_method&) = delete;
	rndr_method& operator=(const rndr_method&) = delete;

	struct createInfo {
		passes::createInfo passInfo;
		materials::createInfo materialInfo;
	};
	void create_renderTarget();
	const storage::vault_readAccessor<render_target> get_readAccessor() const noexcept;

private:
	void create_primaryTarget();

private:
	friend struct rndr_method_Access;

	const rhi::core::device& r_device;
	const rhi::primitive::swapchain& r_swapchain;

	passes m_passes;
	materials m_materials;

	storage::vault<render_target> targets;
	storage::vault_writeAccessor<render_target> m_writeAccess;

};