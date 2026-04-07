#pragma once

#include "core/rsrc_core.h"

#include "Signboard/Core/Interfaces/renderer/rendererINF.h"

class RHIContext;

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