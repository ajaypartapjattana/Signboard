#pragma once

#include "Signboard/Core/Containers/storage.h"
#include "Signboard/RHI/rhi.h"

class RHIContext;
class rndr_presentation;

struct passes_Access;

class passes {
public:
	passes(const rhi::creDevice& device);

	struct createInfo {
		const rhi::pmvSwapchain* tu_swapchain = nullptr;
		VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
	};

	uint32_t createRenderPass(const createInfo* info);
	const ctnr::vltView<rhi::pmvRenderPass> read_renderPasses() const noexcept;

	void createFramebuffers(const uint32_t renderPassIndex, std::vector<uint32_t>& framebuffers, const createInfo* info);
	const ctnr::vltView<rhi::pmvFramebuffer> read_framebuffers() const noexcept;

private:
	friend struct passes_Access;

	const rhi::creDevice& r_device;

	ctnr::vault<rhi::pmvRenderPass> m_renderPasses;
	ctnr::vault_writeAccessor<rhi::pmvRenderPass> rp_writeAccess;

	ctnr::vault<rhi::pmvFramebuffer> m_framebuffers;
	ctnr::vault_writeAccessor<rhi::pmvFramebuffer> fb_writeAccess;

};