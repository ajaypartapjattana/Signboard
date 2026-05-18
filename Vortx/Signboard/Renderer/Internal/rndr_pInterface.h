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

	struct renderTarget {
		uint32_t renderPassIndex;
		std::vector<uint32_t> framebufferIndices;
		std::vector<uint32_t> pipelineIndices;
	};

	struct methodView {
		sgb::vltView<rhi::pmvRenderPass> renderPasses;
		sgb::vltView<rhi::pmvFramebuffer> framebuffers;
		sgb::vltView<rhi::pmvPipeline> pipelines;
	};

	struct frame {
		rhi::pmvFence frameInFlight;

		rhi::pmvSemaphore imageAvailable;
		rhi::pmvSemaphore transferFinished;

		rhi::pmvCommandBuffer CMDGraphics;
		rhi::pmvCommandBuffer CMDTransfer;

		frame(const rhi::creDevice& device) noexcept
			:
			frameInFlight(device, true),
			imageAvailable(device),
			transferFinished(device)
		{

		}

	};

}
