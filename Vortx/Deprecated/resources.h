#pragma once

#include "Signboard/Renderer/Resources/rsrc_core.h"

#include "Signboard/Renderer/Internal/rndr_pInterface.h"

namespace rndr {

	class context;

	struct _pAccess;

	class resources {
	public:
		resources(const context& context) noexcept;

		uint32_t allocateMesh(const Model& model);
		uint32_t allocateImage();

	private:
		friend struct _pAccess;

		rsrc_buffers m_bufferControl;
		rsrc_images m_imageControl;

		resource_pool<VkBuffer> m_buffers;

		sgb::vault<rhi::pmvBuffer> m_buffers;
		sgb::vault<rhi::pmvImage> m_images;

		sgb::vault<Mesh> m_meshes;

	};

}
