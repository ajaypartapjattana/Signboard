#pragma once

#include "Signboard/Core/Containers/storage/storage.h"
#include "Signboard/RHI/rhi.h"

class rndr_context;
class rndr_presentation;

struct passes_Access;

class passes {
public:
	passes(const rhi::core::device& device);

	struct createInfo {
		const rhi::primitive::swapchain* tu_swapchain = nullptr;
		VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
	};

	storage::storage_handle create_pass(const createInfo* info);
	const storage::vault_readAccessor<rhi::primitive::renderPass> get_rp_readAccessor() const noexcept;

	void create_framebuffers(const storage::storage_handle passHandle, const createInfo* info, std::vector<storage::storage_handle>& framebuffers);
	const storage::vault_readAccessor<rhi::primitive::framebuffer>get_fb_readAccessor() const noexcept;

private:
	friend struct passes_Access;

	const rhi::core::device& r_device;

	storage::vault<rhi::primitive::renderPass> m_renderPasses;
	storage::vault_writeAccessor<rhi::primitive::renderPass> rp_writeAccess;

	storage::vault<rhi::primitive::framebuffer> m_framebuffers;
	storage::vault_writeAccessor<rhi::primitive::framebuffer> fb_writeAccess;

};