#pragma once

#include "Signboard/Core/Containers/storage.h"
#include "Signboard/RHI/rhi.h"

class passes {
public:
	passes(const rhi::creDevice& device);

	struct createInfo {
		const rhi::pmvSwapchain* tu_swapchain = nullptr;
	};

	void createSwapchainRenderPass(const rhi::pmvSwapchain& swapchain);
	void createSwapchainFramebuffer(const rhi::pmvSwapchain& swapchain);

	const ctnr::vltView<rhi::pmvRenderPass> read_renderPasses() const noexcept;
	const ctnr::vltView<rhi::pmvFramebuffer> read_framebuffers() const noexcept;

private:
	uint32_t createRenderPass(const createInfo* info);
	uint32_t createFramebuffers(const uint32_t renderPassIndex, const createInfo* info);

private:
	const rhi::creDevice& r_device;
	ctnr::vltView<rhi::pmvImage> r_images;
	

	ctnr::vault<rhi::pmvRenderPass> m_renderPasses;
	ctnr::vault<rhi::pmvFramebuffer> m_framebuffers;

};