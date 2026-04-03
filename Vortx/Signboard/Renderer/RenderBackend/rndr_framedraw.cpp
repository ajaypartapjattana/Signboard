#include "rndr_framedraw.h"

#include "rndr_method_Access.h"

rndr_framedraw::rndr_framedraw(const rndr_method& methods, resourceView& resourceRead)
	:
	m_targets(methods.read_renderTargets()),
	m_methodRead(rndr_method_Access::read_methods(methods)),

	m_resourceRead(std::move(resourceRead))
{

}

void rndr_framedraw::drawFrame(const uint32_t target_index, const rhi::pmvCommandBuffer& target_cmd) {
	rhi::pcdCommandBufferRecorder prcdr{ target_cmd };

	for (const renderTarget& target : m_targets) {
		prcdr.begin_renderTarget(
			*m_methodRead.renderPassView.get(target.renderPassIndex), 
			*m_methodRead.framebufferView.get(target.framebufferIndices[target_index])
		);

		for (const uint32_t& pipeline : target.pipelineIndices) {
			prcdr.bind_pipeline(
				*m_methodRead.pipelineView.get(pipeline),
				true
			);

			for (const Mesh& m : m_resourceRead.meshView) {
				prcdr.bind_vertexBuffer(*m_resourceRead.buffersView.get(m.vertexBuffer));
				prcdr.bind_indexBuffer(*m_resourceRead.buffersView.get(m.indexBuffer));

				prcdr.draw();
			}

		}

		prcdr.end_renderTarget();
	}
	
	prcdr.end_recording();
}