#include "rndr_framedraw.h"

#include "rndr_method_Access.h"

rndr_framedraw::rndr_framedraw(const rndr_method& methods)
	:
	m_targets(methods.get_readAccessor()),

	a_renderpassView(rndr_method_Access::get_renderPassView(methods)),
	a_framebufferView(rndr_method_Access::get_framebufferView(methods)),
	a_pipelineView(rndr_method_Access::get_pipelineView(methods))
{

}

void rndr_framedraw::drawFrame(const uint32_t target_index, const rhi::primitive::commandBuffer& target_cmd) {
	rhi::procedure::cmdbuffer_recorder prcdr{ target_cmd };

	for (const render_target& target : m_targets) {
		prcdr.begin_renderTarget(*a_renderpassView.get(target.pass), *a_framebufferView.get(target.framebuffers[target_index]));

		for (const storage::storage_handle& handle : target.pipelines) {
			prcdr.bind_pipeline(*a_pipelineView.get(handle), true);

			prcdr.draw();
		}

		prcdr.end_renderTarget();
	}
	
	prcdr.end_recording();
}