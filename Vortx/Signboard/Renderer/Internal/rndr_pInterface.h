#pragma once

#include "Signboard/Core/Interfaces/renderer/rendererINF.h"
#include "Signboard/Core/core.h"
#include "Signboard/RHI/rhi.h"

namespace rndr {

	struct resourceView	{
		sgb::vltView<rhi::pmvBuffer> buffers;
		sgb::vltView<rhi::pmvImage> images;
		sgb::vltView<Mesh> meshes;
	};

	

	struct methodView {
		sgb::vltView<rhi::pmvRenderPass> renderPasses;
		sgb::vltView<rhi::pmvFramebuffer> framebuffers;
		sgb::vltView<rhi::pmvPipeline> pipelines;
	};

}
