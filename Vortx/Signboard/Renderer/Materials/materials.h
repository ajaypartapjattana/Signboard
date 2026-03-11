#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Core/Containers/storage/storage.h"

class rndr_context;
class rndr_presentation;

class passes;

class materials {
public:
	materials(const rndr_context& context, const rndr_presentation& presentation, const passes& passes);

	struct material {
		uint32_t targetPass_index;
		uint32_t targetSubpass_index;
		rhi::primitive::pipeline pipeline;
	};

private:
	void create_baseMaterial(uint32_t targetPass_index, uint32_t subpass);

	void create_shader(rhi::primitive::shader& tw_shader, const char* path);

private:
	const rhi::core::device& r_device;
	const rhi::primitive::swapchain& r_swapchain;
	const passes& r_passes;

	rhi::primitive::pipelineLayout m_pipelineLayout;

	material m_baseMat;
	storage::vault<material> m_vault;

};