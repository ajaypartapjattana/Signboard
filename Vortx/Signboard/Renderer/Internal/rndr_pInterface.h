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
		rhi::pmvFence inFlight;

		std::vector<rhi::pmvSemaphore> waitSemaphores;
		std::vector<VkPipelineStageFlags> waitStages;
		rhi::pmvSemaphore transferFinished;
		rhi::pmvSemaphore renderFinished;

		rhi::pmvCommandBuffer CMDGraphics;
		rhi::pmvCommandBuffer CMDTransfer;

		uint32_t assignedImage = 0;

		frame(const rhi::creDevice& device) noexcept
			:
			inFlight(device, true),
			renderFinished(device)
		{

		}
	};

}
