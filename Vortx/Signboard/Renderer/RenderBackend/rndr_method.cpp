#include "rndr_method.h"

#include "rndr_context_Access.h"
#include "rndr_presentation_Access.h"

rndr_method::rndr_method(const rndr_context& context, const rndr_presentation& presentation) 
	:
	r_device(rndr_context_Access::get_device(context)),
	r_swapchain(rndr_presentation_Access::get_swapchain(presentation)),

	m_passes(r_device),
	m_materials(r_device, r_swapchain, m_passes),

	m_writeAccess(targets)
{
	create_primaryTarget();
}

void rndr_method::create_primaryTarget() {
	create_renderTarget();
}

void rndr_method::create_renderTarget() {
	passes::createInfo passInfo{};
	passInfo.tu_swapchain = &r_swapchain;

	materials::createInfo matInfo{};
	matInfo.vertShader_path = "shaders/base.vert.spv";
	matInfo.fragShader_path = "shaders/base.frag.spv";

	auto builder = [&](render_target* tgt) {
		render_target& tw_target = *tgt;

		tw_target.pass = m_passes.create_pass(&passInfo);

		m_passes.create_framebuffers(tw_target.pass, &passInfo, tw_target.framebuffers);

		storage::storage_handle pipeHandle = m_materials.create_pipeline(tw_target.pass, 0, &matInfo);
		tw_target.pipelines.push_back(pipeHandle);
	};

	m_writeAccess.construct(builder);

}

const storage::vault_readAccessor<render_target> rndr_method::get_readAccessor() const noexcept {
	storage::vault_readAccessor<render_target> accessor{ targets };
	return accessor;
}