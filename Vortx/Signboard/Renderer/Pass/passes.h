#pragma once

#include "Signboard/Core/Containers/storage.h"
#include "Signboard/RHI/rhi.h"

class passes {
public:
	passes(const rhi::creDevice& device, const ctnr::vltView<rhi::pmvImage>& resourceImages);

	uint32_t createSwapchainRenderPass(const rhi::pmvSwapchain& swapchain);

	struct renderPassCreateInfo {
		std::vector<VkAttachmentDescription> attachments;
		
	};

	uint32_t createRenderPass(const renderPassCreateInfo& info);

	struct framebufferCreateInfo {
		std::vector<uint32_t> attachmentImageIndices;
	};
	
	uint32_t createFramebuffers(const uint32_t renderPassIndex, const framebufferCreateInfo& info);

	const ctnr::vltView<rhi::pmvRenderPass> read_renderPasses() const noexcept;
	const ctnr::vltView<rhi::pmvFramebuffer> read_framebuffers() const noexcept;

private:
	const rhi::creDevice& r_device;
	const ctnr::vltView<rhi::pmvImage>& r_images;
	
	ctnr::vault<rhi::pmvRenderPass> m_renderPasses;
	ctnr::vault<rhi::pmvFramebuffer> m_framebuffers;

};