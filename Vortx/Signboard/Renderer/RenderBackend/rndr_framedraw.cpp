#include "rndr_framedraw.h"

#include "Signboard/Resources/resources_pAccess.h"
#include "rndr_method_Access.h"

rndr_framedraw::rndr_framedraw(const rndr_method& methods, const ResourceView& resources)
	:
	m_targets(methods.read_renderTargets()),
	m_methodRead(rndr_method_Access::read_methods(methods)),

	r_resources(resources)
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

			for (const Mesh& m : r_resources.meshes) {
				prcdr.bind_vertexBuffer(*r_resources.buffers.get(m.vertexBuffer));
				prcdr.bind_indexBuffer(*r_resources.buffers.get(m.indexBuffer));

				prcdr.draw(m.indexCount);
			}

		}

		prcdr.end_renderTarget();
	}
	
	prcdr.end_recording();
}