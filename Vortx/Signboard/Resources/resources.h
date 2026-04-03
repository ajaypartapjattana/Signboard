#pragma once

#include "core/rsrc_core.h"
#include "procedure/rsrc_procedure.h"

#include "Signboard/Core/Interfaces/uni_mesh/model.h"

class RHIContext;

struct Mesh {
	uint32_t vertexBuffer;
	uint32_t indexBuffer;

	size_t vertexOffset;
	size_t indexOffset;

	uint32_t indexCount;
};

struct resourceView {
	ctnr::vltView<rhi::pmvBuffer> buffersView;
	ctnr::vltView<rhi::pmvImage> imagesView;

	ctnr::vltView<Mesh> meshView;
};

class Resources {
public:
	Resources(const RHIContext& context) noexcept;

	uint32_t allocateMesh(const Model& model);

	resourceView read_resources() const noexcept;

private:
	rsrc_buffers m_buffers;
	rsrc_images m_images;

	ctnr::vault<Mesh> m_meshes;

};