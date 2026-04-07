#include "resources.h"

#include "Signboard/Renderer/Context/render_context_Access.h"

Resources::Resources(const RHIContext& context) noexcept
	:
	m_buffers(rndr_context_Access::get_allocator(context)),
	m_images(rndr_context_Access::get_device(context), rndr_context_Access::get_allocator(context))
{

}

uint32_t Resources::allocateMesh(const Model& model) {
	size_t vertCount = model.vertices.size();
	size_t indexCount = model.indices.size();

	size_t _vertBSz = vertCount * sizeof(Vertex);
	size_t _indBSz = indexCount * sizeof(uint32_t);

	rsrc_buffers::createInfo vInfo{};
	vInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	vInfo.memory = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	vInfo.size = _vertBSz;
	vInfo.allocationFlags = 0;

	rsrc_buffers::createInfo iInfo{};
	iInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	iInfo.memory = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	iInfo.size = _indBSz;
	iInfo.allocationFlags = 0;

	auto builder = [&](Mesh* m) {
		Mesh& _mesh = *m;

		_mesh.vertexBuffer = m_buffers.createBuffer(vInfo);
		_mesh.indexBuffer = m_buffers.createBuffer(iInfo);

		_mesh.vertexOffset = 0;
		_mesh.indexOffset = 0;

		_mesh.indexCount = static_cast<uint32_t>(indexCount);
	};
	
	ctnr::vault_writeAccessor l_writer{ m_meshes };
	
	return l_writer.construct(builder);
}

resourceView Resources::read_resources() const noexcept {
	return {
		m_buffers.read_buffers(),
		m_images.read_Images(),
		ctnr::vltView<Mesh>{ m_meshes }
	};
}

