#include "Signboard/Renderer/Internal/rndr_pAccess.h"

namespace rndr {

	creResources::creResources(const creContext& context) noexcept
		:
		m_bufferControl(_pAccess::allocator(context), sgb::vltWrite<rhi::pmvBuffer>{ m_buffers }),
		m_imageControl(_pAccess::device(context), _pAccess::allocator(context), sgb::vltWrite<rhi::pmvImage>{m_images})
	{

	}

	uint32_t creResources::allocateMesh(const Model& model) {
		size_t vertCount = model.vertices.size();
		size_t indexCount = model.indices.size();

		size_t _vertBSz = vertCount * sizeof(Vertex);
		size_t _indBSz = indexCount * sizeof(uint32_t);

		uint32_t vertexBufferHandle = m_bufferControl.allocateVertexBuffer(_vertBSz);
		uint32_t indexBufferHandle = m_bufferControl.allocateIndexBuffer(_indBSz);

		auto builder = [&](Mesh* m) {
			*m = { vertexBufferHandle, indexBufferHandle, 0, 0, static_cast<uint32_t>(indexCount) };
		};

		sgb::vltWrite l_writer{ m_meshes };
		return l_writer.construct(builder);
	}

	uint32_t creResources::allocateImage() {
		return -1;
	}

}
