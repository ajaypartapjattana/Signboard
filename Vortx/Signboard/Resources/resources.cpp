#include "resources.h"

#include "Signboard/Renderer/Context/render_context_Access.h"

Resources::Resources(const RHIContext& context) noexcept
	:
	bufferControl(rndr_context_Access::get_allocator(context), ctnr::vault_writeAccessor<rhi::pmvBuffer>{ m_buffers }),
	imageControl(rndr_context_Access::get_device(context), rndr_context_Access::get_allocator(context), ctnr::vault_writeAccessor<rhi::pmvImage>{m_images})
{

}

uint32_t Resources::allocateMesh(const Model& model) {
	size_t vertCount = model.vertices.size();
	size_t indexCount = model.indices.size();

	size_t _vertBSz = vertCount * sizeof(Vertex);
	size_t _indBSz = indexCount * sizeof(uint32_t);

	auto builder = [&](Mesh* m) {
		Mesh& _mesh = *m;

		_mesh.vertexBuffer = bufferControl.createVertexBuffer(_vertBSz);
		_mesh.indexBuffer = bufferControl.createIndexBuffer(_indBSz);

		_mesh.vertexOffset = 0;
		_mesh.indexOffset = 0;

		_mesh.indexCount = static_cast<uint32_t>(indexCount);
	};
	
	ctnr::vault_writeAccessor l_writer{ m_meshes };
	
	return l_writer.construct(builder);
}

ResourceView Resources::read_resources() const noexcept {
	return {
		ctnr::vltView<rhi::pmvBuffer>{ m_buffers },
		ctnr::vltView<rhi::pmvImage>{m_images},
		ctnr::vltView<Mesh>{m_meshes}
	};
}

