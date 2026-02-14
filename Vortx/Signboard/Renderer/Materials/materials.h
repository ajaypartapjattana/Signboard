#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Core/Containers/storage/storage.h"

#include "Signboard/Renderer/RenderBackend/render_interface.h"

class materials {
public:
	materials(const render_interface& ri);

	struct material {
		uint32_t targetPass_index;
		uint32_t targetSubpass_index;
		rhi::primitive::pipeline pipeline;
	};

private:
	void create_material(uint32_t targetPass_index, uint32_t subpass);

	void create_shader(rhi::primitive::shader& tw_shader, const char* path);

private:
	const rhi::core::device& m_device;
	const rhi::core::swapchain& m_swapchain;

	rhi::primitive::pipelineLayout m_pipelineLayout;


	storage::vault<material> m_vault;

};