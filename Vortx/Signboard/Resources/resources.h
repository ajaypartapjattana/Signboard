#pragma once

#include "core/rsrc_core.h"

#include "Signboard/Core/Interfaces/renderer/rendererINF.h"

class RHIContext;

struct ResourceView {
	ctnr::vltView<rhi::pmvBuffer> buffers;
	ctnr::vltView<rhi::pmvImage> images;

	ctnr::vltView<Mesh> meshes;
};

struct Resources_pAccess;

class Resources {
public:
	Resources(const RHIContext& context) noexcept;

	uint32_t allocateMesh(const Model& model);
	uint32_t allocateImage();

	ResourceView read_resources() const noexcept;

private:
	friend struct Resources_pAccess;

	rsrc_buffers bufferControl;
	rsrc_images imageControl;

	ctnr::vault<rhi::pmvBuffer> m_buffers;
	ctnr::vault<rhi::pmvImage> m_images;

	ctnr::vault<Mesh> m_meshes;

};