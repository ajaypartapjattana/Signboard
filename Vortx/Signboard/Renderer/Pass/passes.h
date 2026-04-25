#pragma once

#include "Signboard/Core/Containers/storage.h"
#include "Signboard/RHI/rhi.h"

class passes {
public:
	passes(const rhi::creDevice& device, const ctnr::vltView<rhi::pmvImage>& resourceImages);

	struct createInfo {
		std::vector<VkFormat> attachmentFormats;
	};

	uint32_t createSwapchainRenderPass(const rhi::pmvSwapchain& swapchain);
	uint32_t createSwapchainFramebuffer(const rhi::pmvSwapchain& swapchain);

	uint32_t createRenderPass(const createInfo& info);
	uint32_t createFramebuffers(const uint32_t renderPassIndex, const createInfo* info);

	const ctnr::vltView<rhi::pmvRenderPass> read_renderPasses() const noexcept;
	const ctnr::vltView<rhi::pmvFramebuffer> read_framebuffers() const noexcept;

private:
	const rhi::creDevice& r_device;
	const ctnr::vltView<rhi::pmvImage>& r_images;
	
	ctnr::vault<rhi::pmvRenderPass> m_renderPasses;
	ctnr::vault<rhi::pmvFramebuffer> m_framebuffers;

};