#pragma once

#include "Signboard/Core/core.h"
#include "Signboard/RHI/rhi.h"

namespace rndr {

	class framebuffers {
	public:
		framebuffers(const rhi::device& device, sgb::vltWrite<rhi::pmvFramebuffer> framebufferWrite, const sgb::vltView<rhi::pmvImage>& images) noexcept;

		uint32_t createFramebuffers(const rhi::pmvRenderPass& renderPass, sgb::span<const uint32_t> imageIndices);

	private:
		const sgb::vltView<rhi::pmvImage>& r_images;

		rhi::pcdFramebufferCreate m_framebufferCreate;
		sgb::vltWrite<rhi::pmvFramebuffer> _wrt;

	};

}