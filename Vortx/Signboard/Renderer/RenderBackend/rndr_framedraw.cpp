#include "rndr_framedraw.h"

#include "rndr_method_Access.h"

rndr_framedraw::rndr_framedraw(const rndr_method& methods)
	:
	m_targets(methods.read_renderTargets()),

	a_renderpassView(rndr_method_Access::get_renderPassView(methods)),
	a_framebufferView(rndr_method_Access::get_framebufferView(methods)),
	a_pipelineView(rndr_method_Access::get_pipelineView(methods))
{

}

void rndr_framedraw::drawFrame(const uint32_t target_index, const rhi::pmvCommandBuffer& target_cmd) {
	rhi::pcdCommandBufferRecorder prcdr{ target_cmd };

	for (const renderTarget& target : m_targets) {
		prcdr.begin_renderTarget(*a_renderpassView.get(target.renderPassIndex), *a_framebufferView.get(target.framebufferIndices[target_index]));

		for (const uint32_t& handle : target.pipelineIndices) {
			prcdr.bind_pipeline(*a_pipelineView.get(handle), true);

			prcdr.draw();
		}

		prcdr.end_renderTarget();
	}
	
	prcdr.end_recording();
}