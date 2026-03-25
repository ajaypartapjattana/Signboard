#pragma once

#include "Signboard/Core/Containers/storage/storage.h"
#include "Signboard/RHI/rhi.h"

class rndr_context;
class rndr_presentation;

struct passes_Access;

class passes {
public:
	passes(const rhi::creDevice& device);

	struct createInfo {
		const rhi::pmvSwapchain* tu_swapchain = nullptr;
		VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
	};

	storage::storage_handle create_pass(const createInfo* info);
	const storage::vault_readAccessor<rhi::pmvRenderPass> get_rp_readAccessor() const noexcept;

	void create_framebuffers(const storage::storage_handle passHandle, const createInfo* info, std::vector<storage::storage_handle>& framebuffers);
	const storage::vault_readAccessor<rhi::pmvFramebuffer>get_fb_readAccessor() const noexcept;

private:
	friend struct passes_Access;

	const rhi::creDevice& r_device;

	storage::vault<rhi::pmvRenderPass> m_renderPasses;
	storage::vault_writeAccessor<rhi::pmvRenderPass> rp_writeAccess;

	storage::vault<rhi::pmvFramebuffer> m_framebuffers;
	storage::vault_writeAccessor<rhi::pmvFramebuffer> fb_writeAccess;

};