#pragma once

#include "Signboard/Core/core.h"
#include "Signboard/RHI/rhi.h"

namespace rndr {

	class passes {
	public:
		passes(const rhi::device& device, sgb::vltWrite<rhi::pmvRenderPass> renderPassWrite) noexcept;

		uint32_t createSwapchainRenderPass(const rhi::creSwapchain& swapchain);
		uint32_t createRenderPass(sgb::span<const VkAttachmentDescription> attachments);

	private:
		sgb::vltWrite<rhi::pmvRenderPass> _wrt;

		rhi::pcdRenderPassCreate m_renderPassCreate;

	};

}
